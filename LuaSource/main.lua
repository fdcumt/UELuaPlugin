function Init(num)
	FMyTest.StaticPrint();
	FMyTest.StaticPrint1();
	local test = FMyTest.CreateSelf();
	LuaPrint.print(type(test));
	test:SetMember(num)
	local testMember = test:GetMember();
	LuaPrint.print(testMember);
	return num+3, 4, true;
end
