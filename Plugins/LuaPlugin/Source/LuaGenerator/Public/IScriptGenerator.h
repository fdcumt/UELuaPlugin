#pragma once

class IScriptGenerator
{
public:
	virtual ~IScriptGenerator() {}
public:
	virtual FString GetKey() = 0;
	virtual bool CanExport() = 0;
	virtual void Export() = 0;
	virtual void SaveToFile() = 0;
};
