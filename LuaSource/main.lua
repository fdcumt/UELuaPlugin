function Init(num)
	local test1Obj = FMyTest1.CreateSelf();
	local testObj = FMyTest.CreateSelf();
	test1Obj:SetMember(num);
	testObj:SetMember(test1Obj);
	LuaPrint.print(test1Obj);
	test1Obj:Print();
	testObj:Print();
	
	test1Obj:SetMember(4);
	
	test1Obj:Print();
	testObj:Print();
	
	return num+3, 4, true;
end
