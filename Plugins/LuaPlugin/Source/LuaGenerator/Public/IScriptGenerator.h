#pragma once

class IScriptGenerator
{
public:
	IScriptGenerator(const FString &OutDir);
	virtual ~IScriptGenerator();

public:
	virtual FString GetKey() const = 0;
	virtual bool CanExport() const = 0;
	virtual void Export() = 0;
	virtual void SaveToFile() = 0;

protected:
	FString m_OutDir;
};
