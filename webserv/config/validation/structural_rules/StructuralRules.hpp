#pragma once

// Base class
#include "webserv/config/validation/structural_rules/AStructuralValidationRule.hpp"

// Concrete structural validation rules
#include "webserv/config/validation/structural_rules/MinimumServerBlocksRule.hpp"
#include "webserv/config/validation/structural_rules/RequiredLocationBlocksRule.hpp"
#include "webserv/config/validation/structural_rules/UniqueServerNamesRule.hpp"

/**
 * Structural Validation Rules for WebServ Configuration
 * 
 * These rules validate the overall structure and relationships
 * between configuration blocks, rather than individual directive values.
 * 
 * Available Rules:
 * 
 * 1. MinimumServerBlocksRule - Ensures global config has minimum number of server blocks
 * 2. RequiredLocationBlocksRule - Ensures each server has minimum number of location blocks  
 * 3. UniqueServerNamesRule - Ensures all server names are unique across configuration
 * 
 * Usage:
 * 
 *   // In ValidationEngine setup
 *   engine.addStructuralRule(std::make_unique<MinimumServerBlocksRule>(1));
 *   engine.addStructuralRule(std::make_unique<RequiredLocationBlocksRule>(1));
 *   engine.addStructuralRule(std::make_unique<UniqueServerNamesRule>());
 * 
 * Each rule inherits from AStructuralValidationRule and can validate at:
 * - Global level (validateGlobal)
 * - Server level (validateServer) 
 * - Location level (validateLocation)
 * 
 * Rules have descriptive names and descriptions set in their constructors.
 */