function Init(num)
	local testObj = FMyTest.CreateSelf();
	testObj:SetMember(10);
	local testNum = testObj:GetMember();
	LuaPrint.print(testNum);
	
	return num+3, 4, true;
end
