<?php
/**
 * Plugin Name: GitHub Commit History
 * Description: A WordPress plugin to display GitHub commit history using GitHub's pagination API. Supports infinite scroll and complete history loading.
 * Version: 2.0
 * Author: Your Name
 */

// Add AJAX handler for loading more commits
add_action('wp_ajax_load_more_commits', 'load_more_commits_ajax');
add_action('wp_ajax_nopriv_load_more_commits', 'load_more_commits_ajax');

// Add AJAX handler for loading all commits
add_action('wp_ajax_load_all_commits', 'load_all_commits_ajax');
add_action('wp_ajax_nopriv_load_all_commits', 'load_all_commits_ajax');

function load_more_commits_ajax() {
    $repo = sanitize_text_field($_POST['repo']);
    $page = intval($_POST['page']);
    $per_page = intval($_POST['per_page']);
    
    if (empty($repo)) {
        wp_die('Invalid repository');
    }
    
    $result = fetch_commits_html($repo, $page, $per_page);
    
    wp_send_json_success(array(
        'html' => $result['html'],
        'has_more' => $result['has_more']
    ));
}

function load_all_commits_ajax() {
    $repo = sanitize_text_field($_POST['repo']);
    $per_page = intval($_POST['per_page']) ?: 100; // Use larger per_page for efficiency
    
    if (empty($repo)) {
        wp_die('Invalid repository');
    }
    
    $result = fetch_all_commits_html($repo, $per_page);
    
    wp_send_json_success(array(
        'html' => $result['html'],
        'total_commits' => $result['total_commits']
    ));
}

// Helper function to fetch ALL commits using GitHub's pagination API
function fetch_all_commits_html($repo, $per_page = 100) {
    $all_commits_html = '';
    $page = 1;
    $total_commits = 0;
    $max_pages = 100; // Safety limit to prevent infinite loops (10,000 commits max)
    
    do {
        $result = fetch_commits_html($repo, $page, $per_page);
        
        if (empty($result['html'])) {
            break; // No more commits or error occurred
        }
        
        $all_commits_html .= $result['html'];
        $total_commits += substr_count($result['html'], 'class=\'commit-line\'');
        
        $page++;
        
        // Safety check to prevent infinite loops
        if ($page > $max_pages) {
            break;
        }
        
    } while ($result['has_more']);
    
    return array(
        'html' => $all_commits_html, 
        'total_commits' => $total_commits
    );
}

// Helper function to fetch and format commits
function fetch_commits_html($repo, $page = 1, $per_page = 10) {
    $url = "https://api.github.com/repos/$repo/commits?per_page=$per_page&page=$page";
    
    $response = wp_remote_get($url, array(
        'headers' => array('User-Agent' => 'WordPress GitHub Plugin')
    ));
    
    if (is_wp_error($response)) {
        return array('html' => '', 'has_more' => false);
    }
    
    $response_code = wp_remote_retrieve_response_code($response);
    if ($response_code !== 200) {
        return array('html' => '', 'has_more' => false);
    }
    
    $commits = json_decode(wp_remote_retrieve_body($response));
    
    if (empty($commits) || !is_array($commits)) {
        return array('html' => '', 'has_more' => false);
    }
    
    // Check GitHub's Link header for pagination info
    $link_header = wp_remote_retrieve_header($response, 'link');
    $has_more = false;
    
    if ($link_header) {
        // GitHub uses Link header with rel="next" to indicate more pages
        // Example: <https://api.github.com/repos/user/repo/commits?page=2>; rel="next"
        $has_more = strpos($link_header, 'rel="next"') !== false;
    }
    
    // If we got fewer commits than requested per_page, there are no more commits
    if (count($commits) < $per_page) {
        $has_more = false;
    }
    
    $output = '';
    foreach ($commits as $index => $commit) {
        $message = esc_html($commit->commit->message);
        $author_name = substr(esc_html($commit->commit->author->name), 0, 1);
        // $date = date('M j, Y', strtotime($commit->commit->author->date));
        $sha = esc_html(substr($commit->sha, 0, 7));
        $commit_url = esc_url($commit->html_url);
        
        $output .= "<div class='commit-line'>
                        <span class='commit-sha'><a href='$commit_url' target='_blank'>$sha</a></span>
                        <span class='commit-message'>$message</span>
                        <span class='commit-meta'>($author_name)</span>
                    </div>";
        
    }
    
    return array('html' => $output, 'has_more' => $has_more);
}

// Add a shortcode to display commit history
// Usage: [github_commit_history repo="username/repo" count="20"]
// Or to load all commits at once: [github_commit_history repo="username/repo" load_all="true"]
function github_commit_history_shortcode($atts) {
    // Default attributes for the shortcode
    $atts = shortcode_atts(
        array(
            'repo' => '', // GitHub repo in format 'username/repo'
            'count' => 10, // Initial number of commits to display (ignored if load_all=true)
            'load_all' => false, // Whether to load all commits at once using pagination API
        ),
        $atts,
        'github_commit_history'
    );

    // If no repo is provided, return an error message
    if (empty($atts['repo'])) {
        return 'Please provide a GitHub repository.';
    }

    $repo = $atts['repo'];
    $count = intval($atts['count']);
    $load_all = filter_var($atts['load_all'], FILTER_VALIDATE_BOOLEAN);

    // Generate unique ID for this instance
    $instance_id = 'github-commits-' . md5($repo . time());
    
    if ($load_all) {
        // Load all commits at once using GitHub's pagination API
        $all_commits_result = fetch_all_commits_html($repo, 100);
        
        if (empty($all_commits_result['html'])) {
            return 'No commits found or repository is empty.';
        }
        
        $output = '<div class="github-commit-history">';
        $output .= '<h3>Commits for ' . esc_html($repo) . '</h3>';
        $output .= '<div class="commit-tree">';
        $output .= $all_commits_result['html'];
        $output .= '</div>';
        $output .= '</div>';
        
        return $output;
    } else {
        // Use progressive loading with infinite scroll
        $initial_result = fetch_commits_html($repo, 1, $count);
        
        if (empty($initial_result['html'])) {
            return 'No commits found or repository is empty.';
        }
        
        // Start the output
        $output = '<div class="github-commit-history" id="' . $instance_id . '" data-repo="' . esc_attr($repo) . '" data-per-page="' . esc_attr($count) . '">';
        $output .= '<h3>Recent Commits for ' . esc_html($repo) . '</h3>';
        $output .= '<div class="commit-tree">';

        // Get initial commits
        $output .= $initial_result['html'];

        $output .= '</div>';
        
        // Add loading indicator and load more trigger only if there are more commits
        if ($initial_result['has_more']) {
            $output .= '<div class="loading-indicator" style="display: none;">
                            <span class="tree-connector">│</span>
                            <span class="loading-text">Loading more commits...</span>
                        </div>';
            
            $output .= '<div class="load-more-trigger" style="height: 1px;"></div>';
        }
        
        $output .= '</div>';

        // Add the JavaScript for infinite scrolling
        $output .= github_commit_history_script($instance_id);

        return $output;
    }
}

// Register the shortcode with WordPress
add_shortcode('github_commit_history', 'github_commit_history_shortcode');

// JavaScript for infinite scrolling
function github_commit_history_script($instance_id) {
    return '
    <script>
    (function() {
        const container = document.getElementById("' . $instance_id . '");
        if (!container) return;
        
        const commitTree = container.querySelector(".commit-tree");
        const loadingIndicator = container.querySelector(".loading-indicator");
        const loadMoreTrigger = container.querySelector(".load-more-trigger");
        
        const repo = container.dataset.repo;
        const perPage = parseInt(container.dataset.perPage) || 10;
        let currentPage = 1;
        let isLoading = false;
        let hasMore = !!container.querySelector(".load-more-trigger"); // Check if load more trigger exists
        
        // Intersection Observer for infinite scroll (only if load more trigger exists)
        if (loadMoreTrigger && hasMore) {
            const observer = new IntersectionObserver((entries) => {
                entries.forEach(entry => {
                    if (entry.isIntersecting && !isLoading && hasMore) {
                        loadMoreCommits();
                    }
                });
            }, {
                rootMargin: "100px"
            });
            
            observer.observe(loadMoreTrigger);
        }
        
        function loadMoreCommits() {
            if (isLoading || !hasMore) return;
            
            isLoading = true;
            currentPage++;
            loadingIndicator.style.display = "block";
            
            const formData = new FormData();
            formData.append("action", "load_more_commits");
            formData.append("repo", repo);
            formData.append("page", currentPage);
            formData.append("per_page", perPage);
            
            fetch("' . admin_url('admin-ajax.php') . '", {
                method: "POST",
                body: formData
            })
            .then(response => response.json())
            .then(data => {
                if (data.success && data.data.html) {
                    commitTree.insertAdjacentHTML("beforeend", data.data.html);
                    hasMore = data.data.has_more;
                } else {
                    hasMore = false;
                }
            })
            .catch(error => {
                console.error("Error loading more commits:", error);
                hasMore = false;
            })
            .finally(() => {
                isLoading = false;
                loadingIndicator.style.display = "none";
                
                if (!hasMore) {
                    // Add final tree terminator
                    const lastLine = commitTree.querySelector(".tree-line:last-child");
                    if (lastLine) {
                        lastLine.innerHTML = " "; // Replace with empty space
                    }
                }
            });
        }
    })();
    </script>
    ';
}

// Optional: Enqueue a simple style for the output
function github_commit_history_styles() {
    echo <<<CSS
    <style>
        /* General styles for the commit history section - Tokyo Night NGINY theme */
        .github-commit-history {
            font-family: "Fira Code", "JetBrains Mono", "Consolas", "Monaco", "Courier New", monospace;
            background: linear-gradient(135deg, #1a1b26 0%, #16161e 100%);
            color: #a9b1d6;
            padding: 20px;
            border-radius: 12px;
            margin-top: 20px;
            max-height: 500px;
            overflow: auto;
            border: 1px solid #414868;
            box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3);
            position: relative;
        }

        .github-commit-history::before {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            height: 2px;
            background: linear-gradient(90deg, #7aa2f7, #bb9af7, #f7768e, #73daca);
            border-radius: 12px 12px 0 0;
        }

        .github-commit-history h3 {
            font-size: 1.4em;
            margin-bottom: 15px;
            background: linear-gradient(135deg, #7aa2f7 0%, #bb9af7 50%, #f7768e 100%);
            background-clip: text;
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            font-weight: 600;
            text-shadow: 0 0 20px rgba(122, 162, 247, 0.3);
        }

        /* Tree container */
        .commit-tree {
            font-size: 14px;
            line-height: 1.4;
        }

        /* Individual commit line */
        .commit-line {
            display: flex;
            align-items: center;
            white-space: nowrap;
            padding: 4px 8px;
            margin: 1px 0;
            transition: all 0.2s ease;
            border-radius: 6px;
        }

        .commit-line:hover {
            background: rgba(122, 162, 247, 0.1);
            border-left: 3px solid #7aa2f7;
            padding-left: 5px;
            transform: translateX(2px);
        }

        /* Tree connector symbols */
        .tree-connector {
            color: #565f89;
            font-weight: bold;
            margin-right: 8px;
            min-width: 20px;
            text-shadow: 0 0 5px rgba(86, 95, 137, 0.5);
        }

        /* Vertical tree line */
        .tree-line {
            color: #565f89;
            font-weight: bold;
            margin-left: 9px;
            height: 2px;
            line-height: 2px;
        }

        /* Commit SHA styling */
        .commit-sha {
            color: #73daca;
            font-weight: 700;
            margin-right: 12px;
            min-width: 70px;
            text-shadow: 0 0 10px rgba(115, 218, 202, 0.4);
        }

        .commit-sha a {
            color: #73daca;
            text-decoration: none;
            transition: all 0.2s ease;
        }

        .commit-sha a:hover {
            color: #9ece6a;
            text-shadow: 0 0 15px rgba(158, 206, 106, 0.6);
            text-decoration: none;
        }

        /* Commit message */
        .commit-message {
            color: #c0caf5;
            margin-right: 15px;
            flex: 1;
            overflow: hidden;
            text-overflow: ellipsis;
            font-weight: 400;
        }

        /* Commit metadata (author and date) */
        .commit-meta {
            color: #bb9af7;
            font-style: italic;
            white-space: nowrap;
            font-weight: 500;
            text-shadow: 0 0 8px rgba(187, 154, 247, 0.3);
        }

        /* Loading indicator */
        .loading-indicator {
            display: flex;
            align-items: center;
            padding: 15px 0;
            animation: neon-pulse 2s ease-in-out infinite;
        }
        
        .loading-text {
            color: #7aa2f7;
            font-style: italic;
            margin-left: 10px;
            text-shadow: 0 0 10px rgba(122, 162, 247, 0.5);
        }
        
        @keyframes neon-pulse {
            0% { 
                opacity: 0.7;
                text-shadow: 0 0 5px rgba(122, 162, 247, 0.3);
            }
            50% { 
                opacity: 1;
                text-shadow: 0 0 20px rgba(122, 162, 247, 0.8), 0 0 30px rgba(187, 154, 247, 0.4);
            }
            100% { 
                opacity: 0.7;
                text-shadow: 0 0 5px rgba(122, 162, 247, 0.3);
            }
        }
        
        /* Load more trigger (invisible) */
        .load-more-trigger {
            width: 100%;
            pointer-events: none;
        }
        
        /* Custom scrollbar for Tokyo Night theme */
        .github-commit-history::-webkit-scrollbar {
            width: 8px;
        }
        
        .github-commit-history::-webkit-scrollbar-track {
            background: #16161e;
            border-radius: 4px;
        }
        
        .github-commit-history::-webkit-scrollbar-thumb {
            background: linear-gradient(135deg, #7aa2f7, #bb9af7);
            border-radius: 4px;
            box-shadow: 0 0 10px rgba(122, 162, 247, 0.3);
        }
        
        .github-commit-history::-webkit-scrollbar-thumb:hover {
            background: linear-gradient(135deg, #bb9af7, #f7768e);
            box-shadow: 0 0 15px rgba(187, 154, 247, 0.5);
        }

        /* Responsive behavior */
        @media (max-width: 768px) {
            .commit-line {
                flex-direction: column;
                align-items: flex-start;
                white-space: normal;
            }
            
            .commit-meta {
                margin-left: 28px;
                margin-top: 2px;
            }
            
            .loading-indicator {
                flex-direction: column;
                align-items: flex-start;
            }
            
            .loading-text {
                margin-left: 28px;
                margin-top: 2px;
            }
        }
    </style>
CSS;
}
add_action('wp_head', 'github_commit_history_styles');