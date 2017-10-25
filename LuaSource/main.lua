function Init(pBaseLuaWidget)

--[[ test export TEnumAsByte
	local BaseStruct1 = FBaseStruct1.New();
	BaseStruct1.m_EnumAsByte = 0;
	LuaPrint.print("BaseStruct1.m_EnumAsByte"..BaseStruct1.m_EnumAsByte);
	]]

--[[ test export enum
	local BaseStruct1 = FBaseStruct1.New();
	BaseStruct1.m_JustEnum = 1;
	LuaPrint.print("BaseStruct1.m_JustEnum"..BaseStruct1.m_JustEnum);
	]]

--[[ test tmap 
	local BaseStruct1 = FBaseStruct1.New();
	local BaseStruct  = FBaseStruct.New();
	BaseStruct.m = 11;
	--BaseStruct1.m_SetBaseStruct:Add(BaseStruct);
	local bContain = BaseStruct1.m_SetBaseStruct:Contains(BaseStruct);
	LuaPrint.print("bContain"..tostring(bContain));
	]]
	

--[[ test update widget
	--local widgetTree = UWidgetTree.New(pBaseLuaWidget, "WidgetTree")
    --pBaseLuaWidget.WidgetTree = widgetTree;
    --local button = UButton.New(widgetTree, "Button");
    --widgetTree.RootWidget = button;
	]]
	
	
	--[[
	local BaseStruct1 = FBaseStruct1.New();
	local BaseStruct  = FBaseStruct.New();
	BaseStruct.m = 11;
	BaseStruct1.m_MapBaseStruct:Add(6, BaseStruct);
	local a = BaseStruct1.m_MapBaseStruct:Find(6);
	a.m = 33;
	local b = BaseStruct1.m_MapBaseStruct:Find(6);
	local bContain = BaseStruct1.m_MapBaseStruct:Contains(6);
	
	LuaPrint.print("b.m"..b.m);
	LuaPrint.print("bContain"..tostring(bContain));
	
	--BaseStruct1.m_BaseStructs:Add(BaseStruct);
	--
	--local BaseStructTemp = BaseStruct1.m_BaseStructs:Get(0);
	--BaseStructTemp.m = 19;
	--local BaseStructTemp1 = BaseStruct1.m_BaseStructs:Get(0);
	--
	]]


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
	
	]]
end
