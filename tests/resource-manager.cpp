/*
 * This file is part of rosa.
 *
 *  rosa is free software: you can redistribute it and/or modify it under the terms of the
 *  GNU General Public License as published by the Free Software Foundation, either version
 *  3 of the License, or (at your option) any later version.
 *
 *  rosa is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 *  even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with rosa. If not,
 *  see <https://www.gnu.org/licenses/>.
 */

#include <snitch/snitch.hpp>
#include <core/ResourceManager.hpp>

TEST_CASE("Fails to load a non-existent asset pack", "[resource-manager]") {

    auto& rm = rosa::ResourceManager::getInstance();

    REQUIRE_THROWS_AS(rm.registerAssetPack("nonexistent.pak", ""), rosa::MissingPackException);
}

TEST_CASE("Fails to load an asset pack with no manifest", "[resource-manager]") {

    auto& rm = rosa::ResourceManager::getInstance();

    REQUIRE_THROWS_AS(rm.registerAssetPack("packs/nomanifest.pak", ""), rosa::MissingManifestException);
}

TEST_CASE("Fails to load an asset pack with missing assets", "[resource-manager]") {

    auto& rm = rosa::ResourceManager::getInstance();

    REQUIRE_THROWS_AS(rm.registerAssetPack("packs/missingfile.pak", ""), rosa::ResourceNotFoundException);
    REQUIRE_NOTHROW(rm.unregisterAssetPack("packs/missingfile.pak"));
}

TEST_CASE("Retrieve a texture from the example pack and verify size", "[resource-manager]") {

    auto& rm = rosa::ResourceManager::getInstance();
    REQUIRE_NOTHROW(rm.registerAssetPack("../examples/base.pak", ""));

    auto& texture = rm.getAsset<rosa::Texture>(rosa::Uuid("f7055f226bfa1a3b4dbdb366dd18866d"));

    REQUIRE(texture.getSize() == glm::vec2(256, 256));

    REQUIRE_NOTHROW(rm.unregisterAssetPack("../examples/base.pak"));
}

TEST_CASE("Fails to load a texture with a missing DDS header", "[resource-manager]") {

    auto& rm = rosa::ResourceManager::getInstance();

    REQUIRE_THROWS_AS(rm.registerAssetPack("packs/notadds.pak", ""), rosa::MalformedDDSException);
    REQUIRE_NOTHROW(rm.unregisterAssetPack("packs/notadds.pak"));
}

TEST_CASE("Fails to load a texture from an empty file", "[resource-manager]") {

    auto& rm = rosa::ResourceManager::getInstance();

    REQUIRE_THROWS_AS(rm.registerAssetPack("packs/emptydds.pak", ""), rosa::MalformedDDSException);
    REQUIRE_NOTHROW(rm.unregisterAssetPack("packs/emptydds.pak"));
}

TEST_CASE("Fails to unload a non-existent pack", "[resource-manager]") {

    auto& rm = rosa::ResourceManager::getInstance();

    REQUIRE_NOTHROW(rm.registerAssetPack("../examples/base.pak", ""));
    REQUIRE_THROWS_AS(rm.unregisterAssetPack("base.pak"), rosa::UnmountFailedException);

    REQUIRE_NOTHROW(rm.unregisterAssetPack("../examples/base.pak"));
}