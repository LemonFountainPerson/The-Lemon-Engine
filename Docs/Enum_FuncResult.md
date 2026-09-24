# FuncResult

## Definition
```
typedef enum FunctionResult 
{
	LEMON_ERROR = -1,
	MISSING_DATA = -2,
	INVALID_DATA = -3,
	TASK_FAILED = -4,
	LEMON_SUCCESS = 0,
	ACTION_DISABLED = 1,
	EXECUTION_UNNECESSARY = 2,
	AT_FULL_CAPACITY = 3,
	FILE_NOT_FOUND = 4,
	END_OF_FILE = 5
} FuncResult;
```

## Description
The standard return value for functions with no other data to return. Used to indicate whether the operation was 
successful, and if not the general reason why.

## Version
Available since V0.04.


----
