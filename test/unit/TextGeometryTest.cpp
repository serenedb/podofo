// SPDX-FileCopyrightText: 2026 SereneDB GmbH
// SPDX-License-Identifier: MIT-0

#include <catch.hpp>
#include <podofo/main/PdfMemDocument.h>
#include <podofo/main/PdfObjectStream.h>
#include <podofo/main/PdfPageCollection.h>
#include <podofo/main/PdfResources.h>

using namespace PoDoFo;

TEST_CASE("CharacterSpacingSeparatesColumnsWithoutLiteralSpaces")
{
    PdfMemDocument document;
    auto& page = document.GetPages().CreatePage(Rect(0, 0, 300, 300));
    auto& font = document.GetObjects().CreateDictionaryObject(PdfName("Font"));
    font.GetDictionary().AddKey(PdfName("Subtype"), PdfName("Type1"));
    font.GetDictionary().AddKey(PdfName("BaseFont"), PdfName("Helvetica"));
    auto& canvas = static_cast<PdfCanvas&>(page);
    static_cast<PdfResourceOperations&>(canvas.GetOrCreateResources())
        .AddResource(PdfResourceType::Font, PdfName("F1"), font);
    const std::string commands =
        "BT /F1 1 Tf 4.1107 Tc 5.76 0 0 5.76 20 100 Tm "
        "[(02) 4125 (,) 4098 (2) 4125 (4) 4125 (81) 4125 (,) 4098 (1) 4125 (3) 4125 (5)] TJ ET";
    canvas.GetOrCreateContentsStream(PdfStreamAppendFlags::None).SetData(bufferview(commands));
    std::vector<PdfTextEntry> entries;
    PdfTextExtractParams params;
    params.Flags = PdfTextExtractFlags::TokenizeWords | PdfTextExtractFlags::ComputeBoundingBox;
    page.ExtractTextTo(entries, params);
    REQUIRE(entries.size() == 3);
    REQUIRE(entries[0].Text == "0");
    REQUIRE(entries[1].Text == "2,248");
    REQUIRE(entries[2].Text == "1,135");
    REQUIRE(entries[0].X == Approx(20));
    REQUIRE(entries[0].BoundingBox->Width == Approx(3.20256));
    REQUIRE(entries[1].X == Approx(46.880192));
}

TEST_CASE("WordSplittingKeepsTextAndPageUnitsSeparate")
{
    for (double scale : { 1.0, 2.0 })
    {
        PdfMemDocument document;
        auto& page = document.GetPages().CreatePage(Rect(0, 0, 300, 300));
        auto& font = document.GetObjects().CreateDictionaryObject(PdfName("Font"));
        font.GetDictionary().AddKey(PdfName("Subtype"), PdfName("Type1"));
        font.GetDictionary().AddKey(PdfName("BaseFont"), PdfName("Helvetica"));
        auto& canvas = static_cast<PdfCanvas&>(page);
        static_cast<PdfResourceOperations&>(canvas.GetOrCreateResources())
            .AddResource(PdfResourceType::Font, PdfName("F1"), font);
        std::string commands = scale == 1.0 ? "" : "2 0 0 2 0 0 cm ";
        commands += "BT /F1 1 Tf 10 0 0 10 20 100 Tm ( 12 34 ) Tj ET";
        canvas.GetOrCreateContentsStream(PdfStreamAppendFlags::None).SetData(bufferview(commands));
        std::vector<PdfTextEntry> entries;
        PdfTextExtractParams params;
        params.Flags = PdfTextExtractFlags::TokenizeWords | PdfTextExtractFlags::ComputeBoundingBox;
        page.ExtractTextTo(entries, params);
        REQUIRE(entries.size() == 2);
        REQUIRE(entries[0].Text == "12");
        REQUIRE(entries[1].Text == "34");
        REQUIRE(entries[0].X == Approx(22.78 * scale));
        REQUIRE(entries[1].X == Approx(36.68 * scale));
        REQUIRE(entries[0].BoundingBox.has_value());
        REQUIRE(entries[0].BoundingBox->Width == Approx(11.12 * scale));
        entries.clear();
        params.Flags = PdfTextExtractFlags::ComputeBoundingBox;
        page.ExtractTextTo(entries, params);
        REQUIRE(entries.size() == 1);
        REQUIRE(entries[0].Text == "12 34");
        REQUIRE(entries[0].X == Approx(22.78 * scale));
        REQUIRE(entries[0].BoundingBox->Width == Approx(25.02 * scale));
    }
}
