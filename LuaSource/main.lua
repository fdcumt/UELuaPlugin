function Init(num)

--[[
	--LuaPrint.print(testNum);
	local TestBase = FMyTestBase.CreateSelf();
	local Test1 = FMyTest1.CreateSelf();
	local MyTest = FMyTest.CreateSelf();
	TestBase:SetMember(10);
	Test1:SetMember(9);
	MyTest:SetMember(8);
	LuaPrint.print("Test1");
	Test1:GetMember();
	LuaPrint.print("MyTest");
	MyTest:GetMember();
	LuaPrint.print("TestBase");
	TestBase:TestBaseFunc();
	LuaPrint.print("Test1");
	Test1:TestBaseFunc()
	LuaPrint.print("MyTest");
	MyTest:TestBaseFunc()
]]
	local TestBase = FMyTestBase.CreateSelf();
	TestBase.mVariable = 10;
	LuaPrint.print("aaa"..TestBase.mVariable);
	return num+3, 4, true;
end
