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
 *  You should have received a copy of the GNU General Public License along with bbai. If not,
 *  see <https://www.gnu.org/licenses/>.
 */

#include <snitch/snitch.hpp>
#include <core/Uuid.hpp>
#include <unordered_map>

TEST_CASE("Empty Uuid is all zeros", "[uuid]") {

    rosa::Uuid empty_uuid{};

    REQUIRE( empty_uuid.getUpper() == 0x0 && empty_uuid.getLower() == 0x0) ;
}

TEST_CASE("Generate Uuid", "[uuid]") {
    
    rosa::Uuid random_uuid = rosa::Uuid::generate();

    REQUIRE( random_uuid.getUpper() != 0x0 && random_uuid.getLower() != 0x0);
}

TEST_CASE("Parse Uuid from string", "[uuid]") {

    rosa::Uuid uuid_string("ab80206ed1734beb5fc0639fc9c08526");

    REQUIRE( uuid_string.getUpper() == 0xab80206ed1734beb && uuid_string.getLower() == 0x5fc0639fc9c08526 );
}

TEST_CASE("Copy Uuid by constructor", "[uuid]") {

    rosa::Uuid uuid_string("ab80206ed1734beb5fc0639fc9c08526");
    rosa::Uuid copied_uuid(uuid_string);

    REQUIRE( copied_uuid.getUpper() == 0xab80206ed1734beb && copied_uuid.getLower() == 0x5fc0639fc9c08526 );
}

TEST_CASE("Copy Uuid by assignment", "[uuid]") {

    rosa::Uuid uuid_string("ab80206ed1734beb5fc0639fc9c08526");
    rosa::Uuid copied_uuid = uuid_string;

    REQUIRE( copied_uuid.getUpper() == 0xab80206ed1734beb && copied_uuid.getLower() == 0x5fc0639fc9c08526 );
}

TEST_CASE("Explicit cast Uuid to std::string", "[uuid]") {

    rosa::Uuid uuid("ab80206ed1734beb5fc0639fc9c08526");
    std::string str = static_cast<std::string>(uuid);

    REQUIRE( str == "ab80206ed1734beb5fc0639fc9c08526" );
}

TEST_CASE("Uuid as hash key", "[uuid]") {

    std::unordered_map<rosa::Uuid, int> map;

    auto uuid_1 = rosa::Uuid::generate();
    auto uuid_2 = rosa::Uuid::generate();

    map[uuid_1] = 1;
    map[uuid_2] = 2;

    REQUIRE ( map.size() == 2 && map.at(uuid_1) == 1);
}