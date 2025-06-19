#!/usr/bin/env pwsh

# Comprehensive Test Runner for DataLogger Project
# This script builds and runs all tests, providing detailed output and summary

param(
    [switch]$Verbose,
    [switch]$StopOnFirstFailure,
    [string]$BuildDir = "build_release",
    [string]$ConfigType = "Release"
)

Write-Host "=== DataLogger Comprehensive Test Suite ===" -ForegroundColor Cyan
Write-Host "Build Directory: $BuildDir" -ForegroundColor Gray
Write-Host "Configuration: $ConfigType" -ForegroundColor Gray
Write-Host ""

# Ensure we're in the right directory
$ProjectRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $ProjectRoot

# Test results tracking
$TestResults = @{}
$TotalTests = 0
$PassedTests = 0
$FailedTests = 0

# Function to run a single test
function Run-Test {
    param(
        [string]$TestName,
        [string]$TestExecutable
    )

    $global:TotalTests++
    Write-Host "Running $TestName..." -ForegroundColor Yellow

    if (Test-Path $TestExecutable) {
        try {
            $output = & $TestExecutable 2>&1
            $exitCode = $LASTEXITCODE

            if ($exitCode -eq 0) {
                Write-Host "  ✓ PASSED" -ForegroundColor Green
                $global:PassedTests++
                $global:TestResults[$TestName] = @{
                    Status = "PASSED"
                    Output = $output
                    ExitCode = $exitCode
                }

                if ($Verbose) {
                    Write-Host "  Output:" -ForegroundColor Gray
                    $output | ForEach-Object { Write-Host "    $_" -ForegroundColor Gray }
                }
            } else {
                Write-Host "  ✗ FAILED (Exit Code: $exitCode)" -ForegroundColor Red
                $global:FailedTests++
                $global:TestResults[$TestName] = @{
                    Status = "FAILED"
                    Output = $output
                    ExitCode = $exitCode
                }

                Write-Host "  Error Output:" -ForegroundColor Red
                $output | ForEach-Object { Write-Host "    $_" -ForegroundColor Red }

                if ($StopOnFirstFailure) {
                    Write-Host "Stopping on first failure as requested." -ForegroundColor Red
                    exit 1
                }
            }
        } catch {
            Write-Host "  ✗ EXCEPTION: $_" -ForegroundColor Red
            $global:FailedTests++
            $global:TestResults[$TestName] = @{
                Status = "EXCEPTION"
                Output = $_.Exception.Message
                ExitCode = -1
            }
        }
    } else {
        Write-Host "  ✗ TEST EXECUTABLE NOT FOUND" -ForegroundColor Red
        $global:FailedTests++
        $global:TestResults[$TestName] = @{
            Status = "NOT_FOUND"
            Output = "Test executable not found: $TestExecutable"
            ExitCode = -1
        }
    }

    Write-Host ""
}

# Check if build directory exists
if (-not (Test-Path $BuildDir)) {
    Write-Host "Build directory '$BuildDir' not found. Checking alternative locations..." -ForegroundColor Yellow

    # Try common build directory names
    $AlternativeBuildDirs = @("build_release", "build", "bin", "out\build")
    $FoundBuildDir = $null

    foreach ($altDir in $AlternativeBuildDirs) {
        if (Test-Path $altDir) {
            $FoundBuildDir = $altDir
            Write-Host "Found build directory: $altDir" -ForegroundColor Green
            $BuildDir = $altDir
            break
        }
    }

    if (-not $FoundBuildDir) {
        Write-Host "No build directory found. Please build the project first." -ForegroundColor Red
        Write-Host "You can use: cmake --build build_release --config Release" -ForegroundColor Yellow
        exit 1
    }
}

# Define test executables (adjust paths based on your build system)
$TestExecutables = @{
    "Data Structure Tests" = "test_data"
    "NavData Tests" = "test_navdata"
    "SerialOptions Tests" = "test_serialoptions"
    "Shared Memory Tests" = "test_sharedmemory"
    "Swap Tools Tests" = "test_swaptools"
    "Exception Tests" = "test_exceptions"
    "SimpleSerial Tests" = "test_simpleserial"
    "ReadResult Tests" = "test_readresult"
    "Version Tests" = "test_version"
    "Constants Tests" = "test_constants"
    "Integration Tests" = "test_integration"
}

# Alternative paths for different build configurations
$AlternativePaths = @(
    $BuildDir,
    "$BuildDir\$ConfigType",
    "$BuildDir\bin",
    "$BuildDir\bin\$ConfigType",
    "$BuildDir\tests",
    "$BuildDir\tests\$ConfigType",
    "bin",
    ".",
    "tests"
)

# Function to find test executable
function Find-TestExecutable {
    param([string]$BaseName)

    $PossibleNames = @(
        "$BaseName.exe",
        $BaseName,
        "$BaseName.out"
    )

    foreach ($dir in @($BuildDir) + $AlternativePaths) {
        foreach ($name in $PossibleNames) {
            $fullPath = Join-Path $dir $name
            if (Test-Path $fullPath) {
                return $fullPath
            }
        }
    }

    return $null
}

Write-Host "=== Running Individual Tests ===" -ForegroundColor Cyan
Write-Host ""

# Run each test
foreach ($testInfo in $TestExecutables.GetEnumerator()) {
    $testName = $testInfo.Key
    $testBaseName = $testInfo.Value

    # Try to find the executable
    $executablePath = Find-TestExecutable $testBaseName

    if ($executablePath) {
        Run-Test $testName $executablePath
    } else {
        Write-Host "Running $testName..." -ForegroundColor Yellow
        Write-Host "  ⚠ TEST EXECUTABLE NOT FOUND (may need to be built)" -ForegroundColor Yellow
        Write-Host "    Searched for: $testBaseName" -ForegroundColor Gray
        $TotalTests++
        $FailedTests++
        $TestResults[$testName] = @{
            Status = "NOT_BUILT"
            Output = "Test executable needs to be built: $testBaseName"
            ExitCode = -1
        }
        Write-Host ""
    }
}

# Summary
Write-Host "=== Test Summary ===" -ForegroundColor Cyan
Write-Host "Total Tests: $TotalTests" -ForegroundColor White
Write-Host "Passed: $PassedTests" -ForegroundColor Green
Write-Host "Failed: $FailedTests" -ForegroundColor Red

if ($FailedTests -eq 0) {
    Write-Host ""
    Write-Host "🎉 ALL TESTS PASSED! 🎉" -ForegroundColor Green
    Write-Host "The DataLogger project has comprehensive test coverage and all tests are passing." -ForegroundColor Green
} else {
    Write-Host ""
    Write-Host "❌ SOME TESTS FAILED" -ForegroundColor Red
    Write-Host ""
    Write-Host "Failed Tests:" -ForegroundColor Red
    foreach ($result in $TestResults.GetEnumerator()) {
        if ($result.Value.Status -ne "PASSED") {
            Write-Host "  - $($result.Key): $($result.Value.Status)" -ForegroundColor Red
        }
    }
}

Write-Host ""
Write-Host "=== Test Coverage Analysis ===" -ForegroundColor Cyan
Write-Host "The test suite now covers:" -ForegroundColor White
Write-Host "  ✓ Core data structures (Data, NavData)" -ForegroundColor Green
Write-Host "  ✓ Serial communication configuration (SerialOptions)" -ForegroundColor Green
Write-Host "  ✓ Shared memory operations" -ForegroundColor Green
Write-Host "  ✓ Endian conversion utilities" -ForegroundColor Green
Write-Host "  ✓ Exception handling classes" -ForegroundColor Green
Write-Host "  ✓ Serial communication interfaces" -ForegroundColor Green
Write-Host "  ✓ Enums and constants" -ForegroundColor Green
Write-Host "  ✓ Version information" -ForegroundColor Green
Write-Host "  ✓ System constants and macros" -ForegroundColor Green
Write-Host "  ✓ Integration between components" -ForegroundColor Green

Write-Host ""
Write-Host "Components that require hardware for full testing:" -ForegroundColor Yellow
Write-Host "  - Actual serial port communication (requires hardware)" -ForegroundColor Yellow
Write-Host "  - Real-time data logging (requires sensors)" -ForegroundColor Yellow
Write-Host "  - GPS data parsing (requires GPS receiver)" -ForegroundColor Yellow
Write-Host "  - File I/O operations (can be added as file system tests)" -ForegroundColor Yellow

# Exit with appropriate code
exit $FailedTests
