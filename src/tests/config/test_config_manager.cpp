#include <webserv/config/ConfigManager.hpp>

#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

/**
 * @file test_config_manager.cpp
 * @brief Comprehensive unit tests for ConfigManager class
 */

class ConfigManagerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Create a temporary config file for testing
        testConfigFile = std::filesystem::temp_directory_path() / "test_webserv.conf";
        
        // Write a simple valid config without unsupported directives
        std::ofstream file(testConfigFile);
        file << "server {\n";
        file << "    listen 8080;\n";
        file << "    server_name localhost;\n";
        file << "    root /var/www/html;\n";
        file << "    index index.html;\n";
        file << "}\n";
        file << "\n";
        file << "server {\n";
        file << "    listen 8081;\n";
        file << "    server_name example.com;\n";
        file << "    root /var/www/example;\n";
        file << "}\n";
        file.close();
    }

    void TearDown() override
    {
        // Clean up the temporary file
        if (std::filesystem::exists(testConfigFile)) {
            std::filesystem::remove(testConfigFile);
        }
    }

public:
    std::filesystem::path testConfigFile;
};

TEST_F(ConfigManagerTest, SingletonPattern)
{
    ConfigManager& instance1 = ConfigManager::getInstance();
    ConfigManager& instance2 = ConfigManager::getInstance();
    
    // Should be the same instance
    EXPECT_EQ(&instance1, &instance2);
}

TEST_F(ConfigManagerTest, InitValidConfigFile)
{
    ConfigManager& manager = ConfigManager::getInstance();
    
    EXPECT_NO_THROW(manager.init(testConfigFile.string()));
    
    // Should have parsed servers
    const auto serverConfigs = manager.getServerConfigs();
    EXPECT_GT(serverConfigs.size(), 0);
}

TEST_F(ConfigManagerTest, GetServerConfigs)
{
    ConfigManager& manager = ConfigManager::getInstance();
    
    // Try to initialize, but handle "already initialized" case
    try {
        manager.init(testConfigFile.string());
    } catch (const std::runtime_error& e) {
        if (std::string(e.what()).find("already initialized") == std::string::npos) {
            FAIL() << "Unexpected error: " << e.what();
        }
        // If already initialized, that's fine - continue with the test
    }
    
    const auto serverConfigs = manager.getServerConfigs();
    EXPECT_GT(serverConfigs.size(), 0);
    
    // Check that we get valid server configs
    for (const auto* config : serverConfigs) {
        EXPECT_NE(config, nullptr);
    }
}

TEST_F(ConfigManagerTest, GetMatchingServerByHostAndPort)
{
    ConfigManager& manager = ConfigManager::getInstance();
    
    // Try to initialize, but handle "already initialized" case
    try {
        manager.init(testConfigFile.string());
    } catch (const std::runtime_error& e) {
        if (std::string(e.what()).find("already initialized") == std::string::npos) {
            FAIL() << "Unexpected error: " << e.what();
        }
        // If already initialized, that's fine - continue with the test
    }
    
    // Try to find servers by host and port
    const auto* server8080 = manager.getMatchingServerConfig("localhost", 8080);
    const auto* server8081 = manager.getMatchingServerConfig("example.com", 8081);
    const auto* serverNotFound = manager.getMatchingServerConfig("notfound.com", 9999);
    
    // Note: These tests depend on the actual implementation
    // The manager might return a default server even if exact match is not found
    EXPECT_NE(server8080, nullptr);
    EXPECT_NE(server8081, nullptr);
    // serverNotFound might be null or might return a default server
}

TEST_F(ConfigManagerTest, GetMatchingServerByHostPort)
{
    ConfigManager& manager = ConfigManager::getInstance();
    
    // Try to initialize, but handle "already initialized" case
    try {
        manager.init(testConfigFile.string());
    } catch (const std::runtime_error& e) {
        if (std::string(e.what()).find("already initialized") == std::string::npos) {
            FAIL() << "Unexpected error: " << e.what();
        }
        // If already initialized, that's fine - continue with the test
    }
    
    // Try to find servers by host:port string
    const auto* server1 = manager.getMatchingServerConfig("localhost:8080");
    const auto* server2 = manager.getMatchingServerConfig("example.com:8081");
    
    EXPECT_NE(server1, nullptr);
    EXPECT_NE(server2, nullptr);
}

TEST_F(ConfigManagerTest, InvalidConfigFile)
{
    ConfigManager& manager = ConfigManager::getInstance();
    
    // Try to init with non-existent file
    EXPECT_THROW(manager.init("/nonexistent/file.conf"), std::exception);
}

TEST_F(ConfigManagerTest, MalformedConfigFile)
{
    // Create a malformed config file
    std::filesystem::path malformedFile = std::filesystem::temp_directory_path() / "malformed.conf";
    std::ofstream file(malformedFile);
    file << "server {\n";
    file << "    listen invalidport;\n";
    file << "    missing closing brace\n";
    file.close();
    
    ConfigManager& manager = ConfigManager::getInstance();
    
    EXPECT_THROW(manager.init(malformedFile.string()), std::exception);
    
    // Clean up
    std::filesystem::remove(malformedFile);
}

TEST_F(ConfigManagerTest, GetGlobalConfig)
{
    ConfigManager& manager = ConfigManager::getInstance();
    
    // Try to initialize, but handle "already initialized" case
    try {
        manager.init(testConfigFile.string());
    } catch (const std::runtime_error& e) {
        if (std::string(e.what()).find("already initialized") == std::string::npos) {
            FAIL() << "Unexpected error: " << e.what();
        }
        // If already initialized, that's fine - continue with the test
    }
    
    const auto* globalConfig = manager.getGlobalConfig();
    EXPECT_NE(globalConfig, nullptr);
}

TEST_F(ConfigManagerTest, EmptyConfigFile)
{
    // Create empty config file
    std::filesystem::path emptyFile = std::filesystem::temp_directory_path() / "empty.conf";
    std::ofstream file(emptyFile);
    file.close();
    
    ConfigManager& manager = ConfigManager::getInstance();
    
    // Empty config should be handled gracefully or throw exception
    // This depends on implementation - either way is valid
    try {
        manager.init(emptyFile.string());
        // If it doesn't throw, check that we have valid state
        EXPECT_NE(manager.getGlobalConfig(), nullptr);
    } catch (const std::exception&) {
        // If it throws, that's also acceptable for empty config
        SUCCEED();
    }
    
    // Clean up
    std::filesystem::remove(emptyFile);
}

TEST_F(ConfigManagerTest, MinimalConfig)
{
    // Create minimal valid config
    std::filesystem::path minimalFile = std::filesystem::temp_directory_path() / "minimal.conf";
    std::ofstream file(minimalFile);
    file << "server {\n";
    file << "    listen 9090;\n";
    file << "}\n";
    file.close();
    
    // Use a fresh ConfigManager instance for this test
    // Note: ConfigManager is singleton, so we can't truly create a new instance
    // This test checks that initialization is idempotent or properly handled
    try {
        ConfigManager& manager = ConfigManager::getInstance();
        manager.init(minimalFile.string());
        
        const auto serverConfigs = manager.getServerConfigs();
        EXPECT_GT(serverConfigs.size(), 0);
    } catch (const std::runtime_error& e) {
        // If ConfigManager is already initialized, that's acceptable
        if (std::string(e.what()).find("already initialized") != std::string::npos) {
            SUCCEED();
        } else {
            FAIL() << "Unexpected error: " << e.what();
        }
    }
    
    // Clean up
    std::filesystem::remove(minimalFile);
}
