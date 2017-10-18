function Init(pProjectMode, pBaseStruct1)

	pProjectMode.InputPriority = 1111;
	LuaPrint.print("pProjectMode.InputPriority"..pProjectMode.InputPriority);
	
	pBaseStruct1.m_Value1=222;
	LuaPrint.print("pBaseStruct1.m_Value1"..pBaseStruct1.m_Value1);

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
	local baseStruct = FBaseStruct.New();
	local mStruct = FBaseStruct1.Get_m_Struct(baseStruct1);
	baseStruct.m = 90;
	baseStruct1.m_Struct = baseStruct;
	LuaPrint.print("baseStruct1.m_Struct.m,"..baseStruct1.m_Struct.m);
	baseStruct1.m_Struct.m = 12;
	LuaPrint.print("baseStruct1.m_Struct.m,"..baseStruct1.m_Struct.m);
	mStruct.m = 14;
	LuaPrint.print("mStruct.m:"..mStruct.m);
	LuaPrint.print("baseStruct1.m_Struct.m,"..baseStruct1.m_Struct.m);
	--local baseStruct = FBaseStruct.New();
	--baseStruct.m = 33;
	--FBaseStruct1.Set_m_Value1(baseStruct1, 10);
	--local member = FBaseStruct1.Get_m_Struct(baseStruct1);
	 
	--local temp = baseStruct1.m_Struct;
	--member.m = 10;
	--local member1 = FBaseStruct1.Get_m_Struct(baseStruct1);
	--baseStruct1.m_Value1 = 32;
	--member1.m = 15;
	--baseStruct2.m_Value1 = 35;
	--local temp = baseStruct2.m_Struct;
	--baseStruct1.m_Value3 = 38;
	--temp.m = 11.0;
	
	--LuaPrint.print("baseStruct1.m_Value1,"..member.m);
	--LuaPrint.print("baseStruct2.m_Value1,"..baseStruct2.m_Value1);
	--LuaPrint.print(""..baseStruct1.m_Value3);
	--LuaPrint.print("baseStruct1.m_Struct.m"..temp.m);
	--LuaPrint.print("baseStruct1.m_Struct.m"..baseStruct1.m_Struct.m);
	--local pWidget = UWidget.New(pProjectMode, "UWidget");
	--pWidget.bIsVariable = false;
	--LuaPrint.print("pWidget.bIsVariable"..pWidget.bIsVariable);
	
	return 3, 4, true;
end
