<?php
/**
 * Plugin Name: GitHub Commit History
 * Description: A WordPress plugin to display GitHub commit history.
 * Version: 1.1
 * Author: Your Name
 */

// Add AJAX handler for loading more commits
add_action('wp_ajax_load_more_commits', 'load_more_commits_ajax');
add_action('wp_ajax_nopriv_load_more_commits', 'load_more_commits_ajax');

function load_more_commits_ajax() {
    $repo = sanitize_text_field($_POST['repo']);
    $page = intval($_POST['page']);
    $per_page = intval($_POST['per_page']);
    
    if (empty($repo)) {
        wp_die('Invalid repository');
    }
    
    $commits_html = fetch_commits_html($repo, $page, $per_page);
    
    wp_send_json_success(array(
        'html' => $commits_html,
        'has_more' => !empty($commits_html) // Simple check - if we got HTML, there might be more
    ));
}

// Helper function to fetch and format commits
function fetch_commits_html($repo, $page = 1, $per_page = 10) {
    $url = "https://api.github.com/repos/$repo/commits?per_page=$per_page&page=$page";
    
    $response = wp_remote_get($url, array(
        'headers' => array('User-Agent' => 'WordPress GitHub Plugin')
    ));
    
    if (is_wp_error($response)) {
        return '';
    }
    
    $commits = json_decode(wp_remote_retrieve_body($response));
    
    if (empty($commits)) {
        return '';
    }
    
    $output = '';
    foreach ($commits as $index => $commit) {
        $message = esc_html($commit->commit->message);
        $author_name = esc_html($commit->commit->author->name);
        $date = date('M j, Y', strtotime($commit->commit->author->date));
        $sha = esc_html(substr($commit->sha, 0, 7));
        $commit_url = esc_url($commit->html_url);
        
        $connector = '├─'; // For infinite scroll, we always use the branch connector
        
        $output .= "<div class='commit-line'>
                        <span class='tree-connector'>$connector</span>
                        <span class='commit-sha'><a href='$commit_url' target='_blank'>$sha</a></span>
                        <span class='commit-message'>$message</span>
                        <span class='commit-meta'>($author_name, $date)</span>
                    </div>";
        
        $output .= "<div class='tree-line'>│</div>";
    }
    
    return $output;
}

// Add a shortcode to display commit history
function github_commit_history_shortcode($atts) {
    // Default attributes for the shortcode
    $atts = shortcode_atts(
        array(
            'repo' => '', // GitHub repo in format 'username/repo'
            'count' => 10, // Initial number of commits to display
        ),
        $atts,
        'github_commit_history'
    );

    // If no repo is provided, return an error message
    if (empty($atts['repo'])) {
        return 'Please provide a GitHub repository.';
    }

    // Build GitHub API URL
    $repo = $atts['repo'];
    $count = intval($atts['count']);
    $url = "https://api.github.com/repos/$repo/commits?per_page=$count";

    // Fetch commits from GitHub API
    $response = wp_remote_get($url, array(
        'headers' => array('User-Agent' => 'WordPress GitHub Plugin')
    ));

    // Check if the API request was successful
    if (is_wp_error($response)) {
        return 'Error fetching commit data from GitHub.';
    }

    // Parse the response body as JSON
    $commits = json_decode(wp_remote_retrieve_body($response));

    if (empty($commits)) {
        return 'No commits found or repository is empty.';
    }

    // Generate unique ID for this instance
    $instance_id = 'github-commits-' . md5($repo . time());
    
    // Start the output
    $output = '<div class="github-commit-history" id="' . $instance_id . '" data-repo="' . esc_attr($repo) . '" data-per-page="' . esc_attr($count) . '">';
    $output .= '<h3>Recent Commits for ' . esc_html($repo) . '</h3>';
    $output .= '<div class="commit-tree">';

    // Get initial commits
    $initial_commits = fetch_commits_html($repo, 1, $count);
    $output .= $initial_commits;

    $output .= '</div>';
    
    // Add loading indicator and load more trigger
    $output .= '<div class="loading-indicator" style="display: none;">
                    <span class="tree-connector">│</span>
                    <span class="loading-text">Loading more commits...</span>
                </div>';
    
    $output .= '<div class="load-more-trigger" style="height: 1px;"></div>';
    $output .= '</div>';

    // Add the JavaScript for infinite scrolling
    $output .= github_commit_history_script($instance_id);

    return $output;
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
        let hasMore = true;
        
        // Intersection Observer for infinite scroll
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
    echo '
    <style>
        /* General styles for the commit history section */
        .github-commit-history {
            font-family: "Consolas", "Monaco", "Courier New", monospace;
            background-color: #1e1e1e;
            color: #d4d4d4;
            padding: 20px;
            border-radius: 8px;
            margin-top: 20px;
            max-height: 500px;
            overflow: auto;
            border: 1px solid #3c3c3c;
        }

        .github-commit-history h3 {
            font-size: 1.4em;
            margin-bottom: 15px;
            color: #569cd6;
            font-weight: normal;
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
            padding: 2px 0;
            transition: background-color 0.2s ease;
        }

        .commit-line:hover {
            background-color: #2d2d30;
            border-radius: 3px;
        }

        /* Tree connector symbols */
        .tree-connector {
            color: #808080;
            font-weight: bold;
            margin-right: 8px;
            min-width: 20px;
        }

        /* Vertical tree line */
        .tree-line {
            color: #808080;
            font-weight: bold;
            margin-left: 9px;
            height: 2px;
            line-height: 2px;
        }

        /* Commit SHA styling */
        .commit-sha {
            color: #f9e71e;
            font-weight: bold;
            margin-right: 10px;
            min-width: 60px;
        }

        .commit-sha a {
            color: #f9e71e;
            text-decoration: none;
        }

        .commit-sha a:hover {
            text-decoration: underline;
        }

        /* Commit message */
        .commit-message {
            color: #d4d4d4;
            margin-right: 15px;
            flex: 1;
            overflow: hidden;
            text-overflow: ellipsis;
        }

        /* Commit metadata (author and date) */
        .commit-meta {
            color: #608b4e;
            font-style: italic;
            white-space: nowrap;
        }

        /* Loading indicator */
        .loading-indicator {
            display: flex;
            align-items: center;
            padding: 10px 0;
            animation: pulse 1.5s ease-in-out infinite;
        }
        
        .loading-text {
            color: #808080;
            font-style: italic;
            margin-left: 10px;
        }
        
        @keyframes pulse {
            0% { opacity: 0.6; }
            50% { opacity: 1; }
            100% { opacity: 0.6; }
        }
        
        /* Load more trigger (invisible) */
        .load-more-trigger {
            width: 100%;
            pointer-events: none;
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
    ';
}
add_action('wp_head', 'github_commit_history_styles');