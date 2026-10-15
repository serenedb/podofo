// SPDX-FileCopyrightText: 2026 SereneDB GmbH
// SPDX-License-Identifier: MIT-0

#include <new>
#include <catch.hpp>
#include <podofo/private/Format.h>

TEST_CASE("RuntimeFormatStrings")
{
    std::string pattern = "{}:{:04}";
    REQUIRE(utls::Format(pattern, "page", 7) == "page:0007");
    REQUIRE(COMMON_FORMAT(pattern, "page", 7) == "page:0007");
    REQUIRE(COMMON_FORMAT(std::string("literal {}")) == "literal {}");

    std::string output = "old content";
    utls::FormatTo(output, pattern, "page", 7);
    REQUIRE(output == "page:0007");

    char bounded[] = "......";
    utls::FormatTo(bounded, 4, pattern, "page", 7);
    REQUIRE(std::string(bounded) == "page..");
    utls::FormatTo(bounded, 0, pattern, "other", 9);
    REQUIRE(std::string(bounded) == "page..");

    char shortOutput[] = "......";
    utls::FormatTo(shortOutput, 6, "{}", 42);
    REQUIRE(std::string(shortOutput) == "42....");

    REQUIRE_THROWS_AS(utls::Format(std::string("{"), 7), std::format_error);
}
