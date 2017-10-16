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

	local TestBase = FMyTestBase.CreateSelf();
	TestBase.mVariable = 10;
	LuaPrint.print("aaa"..TestBase.mVariable);


	local TestBase = FMyTestBase.CreateSelf();
	TestBase.mVariable = 10;
	LuaPrint.print("aaa"..TestBase.mVariable);

	
	local hitResult = FHitResult.New();
	hitResult:Set_FaceIndex(20);
	LuaPrint.print("aaa"..hitResult.FaceIndex);
	hitResult:Destory();
]]
--[[
	local baseStruct1 = FBaseStruct1.New();
	local member = baseStruct1.baseStruct;
	member.m = 10;
	LuaPrint.print("aaa"..baseStruct1.baseStruct.m);
	--baseStruct1.baseStruct = baseStruct;
	--baseStruct1.baseStruct.m = 15;

	local a = baseStruct1.baseStruct;
	a.m = 15;
	LuaPrint.print("aaa"..baseStruct1.baseStruct.m);
	baseStruct:Destory();
	baseStruct1:Destory();
	
	]]
	
	local baseStruct1 = FBaseStruct1.New();
	FBaseStruct1.m = 32;
	--local temp = baseStruct1.m_Struct;
	--hitResult.Distance = 11.0;
	
	LuaPrint.print("baseStruct1.m_Value"..baseStruct1.m_Value);
	--LuaPrint.print("hitResult.Distance"..hitResult.Distance);
	
	
	return num+3, 4, true;
end
