# Polonius automated tests

These tests are in place to make sure changes to Polonius's code don't break functionality

The tests can be run via `./run-tests`

 - Results are stored in `results/`
 - Debug information from each test is written to `debug/`
 - Temporary files used by the tests are stored in `tmp/`
 - The test scripts themselves are stored in `scripts/`

To add a new test, create a script with a filename ending in `.sh` and place it in the `scripts/` folder.

The test will be considered **successful** if the **ONLY** output from the script is "0"
