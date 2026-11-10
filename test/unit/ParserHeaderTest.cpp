// SPDX-FileCopyrightText: 2026 SereneDB GmbH
// SPDX-License-Identifier: MIT-0

#include <new>
#include <catch.hpp>
#include <podofo/main/PdfError.h>
#include <podofo/main/PdfMemDocument.h>

TEST_CASE("RejectInvalidPdfHeadersBeforeXRefRecovery")
{
    for (std::string_view input : {"x", "This is not a PDF.", "%PDF-", "%PDF-9.9"})
    {
        PoDoFo::PdfMemDocument document;
        try
        {
            document.LoadFromBuffer(PoDoFo::bufferview(input.data(), input.size()));
            FAIL("Invalid PDF header was accepted");
        }
        catch (const PoDoFo::PdfError& error)
        {
            REQUIRE(error.GetCode() == PoDoFo::PdfErrorCode::InvalidPDF);
        }
    }
}
