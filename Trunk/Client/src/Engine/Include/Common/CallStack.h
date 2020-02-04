
size_t validIndexCount = 0;
void* callStack[DBG_MEM_CALLSTACK_NUM];
{
	DWORD _ebp, _esp;
	__asm mov _ebp, ebp;
	__asm mov _esp, esp;
	size_t index = 0;
	//__try
	{
		for(; index < DBG_MEM_CALLSTACK_NUM; index++)
		{
			callStack[index] = (void*)ULongToPtr(*(((DWORD*)ULongToPtr(_ebp))+1));
			_ebp = *(DWORD*)ULongToPtr(_ebp);
			if(_ebp == 0 || 0 != (_ebp & 0xFC000000) || _ebp < _esp)
				break;
		}
	}
	//__except(EXCEPTION_CONTINUE_EXECUTION)
	//{

	//}
	validIndexCount = index;
	for(; index < DBG_MEM_CALLSTACK_NUM; index++)
	{
		callStack[index] = 0;
	}
}