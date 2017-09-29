function Init(num)
	local test1Obj = FMyTest1.CreateSelf();
	local testObj = FMyTest.CreateSelf();
	test1Obj:SetMember(num);

	testObj:TestConstPoint(test1Obj);
	testObj:TestPointRef(test1Obj);
	testObj:TestConstRef(test1Obj);
	testObj:TestRef(test1Obj);
	
	return num+3, 4, true;
end
