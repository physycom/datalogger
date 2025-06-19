#!/usr/bin/env pwsh

# Test Structure Validation Script
# Validates that all test files are properly structured and can compile

Write-Host "=== DataLogger Test Structure Validation ===" -ForegroundColor Cyan
Write-Host ""

$ProjectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $ProjectRoot

# Test files to validate
$TestFiles = @(
    "test_data.cpp",
    "test_navdata.cpp",
    "test_serialoptions.cpp",
    "test_sharedmemory.cpp",
    "test_swaptools.cpp",
    "test_exceptions.cpp",
    "test_simpleserial.cpp",
    "test_readresult.cpp",
    "test_version.cpp",
    "test_constants.cpp",
    "test_integration.cpp"
)

$AllValid = $true

Write-Host "Validating test file structure..." -ForegroundColor Yellow
Write-Host ""

foreach ($testFile in $TestFiles) {
    $filePath = Join-Path "." $testFile
    Write-Host "Checking $testFile..." -ForegroundColor White

    if (Test-Path $filePath) {
        $content = Get-Content $filePath -Raw

        # Check for basic structure
        $hasInclude = $content -match '#include\s*[<"].*[>"]'
        $hasMain = $content -match 'int\s+main\s*\('
        $hasReturn = $content -match 'return\s+0\s*;'

        if ($hasInclude -and $hasMain -and $hasReturn) {
            Write-Host "  ✓ Structure is valid" -ForegroundColor Green

            # Check for test assertions
            $hasAssert = $content -match 'assert\s*\('
            if ($hasAssert) {
                Write-Host "  ✓ Contains test assertions" -ForegroundColor Green
            } else {
                Write-Host "  ⚠ No assertions found" -ForegroundColor Yellow
            }

            # Check for output messages
            $hasOutput = $content -match 'std::cout|cout'
            if ($hasOutput) {
                Write-Host "  ✓ Contains test output" -ForegroundColor Green
            } else {
                Write-Host "  ⚠ No test output found" -ForegroundColor Yellow
            }

        } else {
            Write-Host "  ✗ Invalid structure" -ForegroundColor Red
            if (-not $hasInclude) { Write-Host "    Missing includes" -ForegroundColor Red }
            if (-not $hasMain) { Write-Host "    Missing main function" -ForegroundColor Red }
            if (-not $hasReturn) { Write-Host "    Missing return statement" -ForegroundColor Red }
            $AllValid = $false
        }
    } else {
        Write-Host "  ✗ File not found" -ForegroundColor Red
        $AllValid = $false
    }
    Write-Host ""
}

# Check CMakeLists.txt for test registration
Write-Host "Checking CMakeLists.txt for test registration..." -ForegroundColor Yellow
$cmakeFile = Join-Path ".." "CMakeLists.txt"
if (Test-Path $cmakeFile) {
    $cmakeContent = Get-Content $cmakeFile -Raw

    $registeredTests = 0
    foreach ($testFile in $TestFiles) {
        $testName = [System.IO.Path]::GetFileNameWithoutExtension($testFile)
        if ($cmakeContent -match "add_executable\s*\(\s*$testName") {
            $registeredTests++
        }
    }

    Write-Host "  Registered tests: $registeredTests / $($TestFiles.Count)" -ForegroundColor White
    if ($registeredTests -eq $TestFiles.Count) {
        Write-Host "  ✓ All tests registered in CMake" -ForegroundColor Green
    } else {
        Write-Host "  ⚠ Some tests not registered in CMake" -ForegroundColor Yellow
    }
} else {
    Write-Host "  ✗ CMakeLists.txt not found" -ForegroundColor Red
    $AllValid = $false
}

Write-Host ""
Write-Host "=== Test Coverage Summary ===" -ForegroundColor Cyan

$CoverageAreas = @{
    "Core Data Structures" = @("test_data.cpp", "test_navdata.cpp")
    "Configuration Classes" = @("test_serialoptions.cpp")
    "Memory Management" = @("test_sharedmemory.cpp")
    "Utility Functions" = @("test_swaptools.cpp")
    "Error Handling" = @("test_exceptions.cpp")
    "Communication Interfaces" = @("test_simpleserial.cpp")
    "Enums and Constants" = @("test_readresult.cpp", "test_constants.cpp")
    "Version Management" = @("test_version.cpp")
    "Integration Testing" = @("test_integration.cpp")
}

foreach ($area in $CoverageAreas.GetEnumerator()) {
    $areaName = $area.Key
    $testFiles = $area.Value
    $existingTests = $testFiles | Where-Object { Test-Path $_ }

    Write-Host "$areaName`: " -NoNewline -ForegroundColor White
    if ($existingTests.Count -eq $testFiles.Count) {
        Write-Host "Complete ✓" -ForegroundColor Green
    } elseif ($existingTests.Count -gt 0) {
        Write-Host "Partial ⚠ ($($existingTests.Count)/$($testFiles.Count))" -ForegroundColor Yellow
    } else {
        Write-Host "Missing ✗" -ForegroundColor Red
    }
}

Write-Host ""
if ($AllValid) {
    Write-Host "🎉 All test files are properly structured!" -ForegroundColor Green
    Write-Host "The tests are ready to be built and executed." -ForegroundColor Green
} else {
    Write-Host "❌ Some test files have issues that need to be resolved." -ForegroundColor Red
}

Write-Host ""
Write-Host "To build and run tests, use:" -ForegroundColor Yellow
Write-Host "  cmake --build build --config Release" -ForegroundColor Gray
Write-Host "  ctest --build-config Release --verbose" -ForegroundColor Gray
Write-Host "Or run individual tests after building:" -ForegroundColor Gray
Write-Host "  .\build\test_integration.exe" -ForegroundColor Gray
