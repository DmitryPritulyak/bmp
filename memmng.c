struct memblock
{
	unsigned length;
	unsigned attributes;
	void *useptr;
	struct memblock *next;
} blocks = {0, 0, NULL, NULL};

void *MM_FindPtr(void *ptr)
{
	struct memblock *temp;
	
	temp = &blocks;
	while(temp->useptr!=ptr)
	{
		temp = temp->next;
		if(!temp->next)
		{
			return NULL;
		}
	}
	return temp;
}

void MM_FreePtr(void *ptr)
{
	struct memblock *temp;
	temp = (struct memblock*)MM_FindPtr(ptr);
	
	if(!ptr || !temp) 
	{
		puts ("MM_FreePtr: Out of memory\n");
		exit(1);
	}
	if(!blocks.useptr)
	{
		puts ("MM_FreePtr: Memory is Empty\n");
		exit(1);	
	}
	*temp = *temp->next;
	free(temp->next);
	free(ptr);
}

void *MM_GetPtr(unsigned length, unsigned attributes)
{
	struct memblock temp;

	void *ptr = (void*)malloc(length);
	temp.next = (struct memblock*)malloc(sizeof(struct memblock));

	if(!ptr || !temp.next)
	{
		puts ("MM_GetPtr: Out of memory\n");
		exit(1);
	}

	temp.length = length;
	temp.attributes = attributes;
	temp.useptr = ptr;
	
	*temp.next = blocks;
	blocks = temp;
	return ptr;
}

void MM_SortMem()
{
	struct memblock *temp;
	
	temp = &blocks;
	while(temp)
	{
		if(temp->attributes == CAN_FREE)
		{
			free(temp->useptr);
			*temp = *temp->next;
			free(temp->next);
		}
		else
			temp = temp->next;
	}
}

void DisplayP()
{
	struct memblock temp;
	temp = blocks;
	while(temp.useptr)
	{
		printf("size=%i\n", temp.length);
		temp=*(temp.next);
	}
}

void MM_ChangePurge(void *ptr)
{
	struct memblock *temp;
	
	if(!ptr) 
	{
		puts ("MM_FreePtr: Out of memory\n");
		exit(1);
	}
	
	temp = (struct memblock*)MM_FindPtr(ptr);
	temp->attributes = CAN_FREE;
}