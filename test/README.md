# The Test Suite for refspec

## "test_response" CLI options

### Mode settings
* ```verbose``` - boolean
* ```cimode``` -- boolean, runs with specialized set of parameter in the CI context

### Parameters
* ```taps_start```, ```taps_end```
* ```notch_start```, ```notch_end```
* ```win_start```, ```win_end```

### Example
```bash
# Generic run with user-defined taps
./test/test_response.exe -v --taps_start 12 --taps_end 16
# CI run (one file in output)
./test/test_response.exe -c
```