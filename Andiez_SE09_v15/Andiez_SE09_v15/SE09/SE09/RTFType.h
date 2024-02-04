#pragma once
#include <Windows.h>
#include <iostream>
#include <ctype.h>
#include <string> 
#include <vector>
#include <map>

using namespace std;

/*
Enum: FontFamily
Description: Represents font families in RTF.
*/

typedef enum
{
	FNIL,        // Unknown or default fonts (the default)
	FROMAN,      // Roman, proportionally spaced serif fonts (e.g., Times New Roman, Palatino)
	FSWISS,      // Swiss, proportionally spaced sans serif fonts (e.g., Arial)
	FMODERN,     // Fixed-pitch serif and sans serif fonts (e.g., Courier New, Pica)
	FSCRIPT,     // Script fonts (e.g., Cursive)
	FDECOR,      // Decorative fonts (e.g., Old English, ITC Zapf Chancery)
	FTECH,       // Technical, symbol, and mathematical fonts (e.g., Symbol)
	FBIDI,       // Arabic, Hebrew, or other bidirectional font (e.g., Miriam)
} FontFamily;

/*
Enum: CodePage
Description: Represents code pages used in RTF.
*/
typedef enum
{
	CP_437,       // IBM Hoa Kỳ
	CP_708,       // Tiếng Ả Rập (ASMO 708)
	CP_709,       // Tiếng Ả Rập (ASMO 449+, BCON V4)
	CP_710,       // Tiếng Ả Rập (tiếng Ả Rập trong suốt)
	CP_711,       // Tiếng Ả Rập (Nafitha nâng cao)
	CP_720,       // Tiếng Ả Rập (ASMO trong suốt)
	CP_819,       // Windows 3.1 (Hoa Kỳ và Tây Âu)
	CP_850,       // IBM đa ngôn ngữ
	CP_852,       // Đông Âu
	CP_860,       // Tiếng Bồ Đào Nha
	CP_862,       // Tiếng Do Thái
	CP_863,       // Người Canada gốc Pháp
	CP_864,       // Tiếng Ả Rập
	CP_865,       // Người Na Uy
	CP_866,       // Liên Xô
	CP_932,       // Tiếng Nhật
	CP_1250,      // Windows 3.1 (Đông Âu)
	CP_1251       // Windows 3.1 (chữ Cyrillic)
} CodePage;

// This structure is used to save state when starting a group and exiting a group
typedef struct Group
{
	string groupName;
	map<string, int> attributes;
	vector<Group> nestedGroups;
	Group(string name) : groupName(name) {}
} GROUP;

//RTF Destination State
typedef enum	
{
	rdsNorm,
	rdsSkip	
} RDS;

//RTF Internal State
typedef enum 	
{
	risNorm,
	risBin,	
	risHex,	
} RIS;

typedef enum
{
	sNorm,
	sSkip,
	hFontTable,		//Font Table
	hColorTable,	//Color Table
	hFileTable,		//File Table
	hStyleSheet,	//StyleSheet
	hListTable,
	dParagraph
} STATE;

typedef struct Save
{
	struct Save* pNext;		//Tham chiếu đến lưu trữ tiếp theo
	RDS rds;				//Trạng thái đích RTF (RTF Destination State)
	RIS ris;				//Trạng thái nội bộ RTF (RTF Internal State)
} SAVE;

// This struct is list the RTF Control Word
typedef struct CtrlWord
{
	string keyword;
	int param;
	bool fParam;
	CtrlWord() : keyword("0"), param(0), fParam(false) {}
} CTRLWORD;

enum ControlType
{
	Flag,			// Control word này sẽ bỏ qua mọi param
	Destination,	// Control word này bắt đầu một group hoặc đích. Nó bỏ qua bất kỳ param nào
	Symbol,			// Control word này đại diện cho một ký tự đặc biệt
	Toggle,			// Control word này dùng để phân biệt state ON/OFF (Not param hoặc parm != 0 => ON, param = 0 => OFF)
	Value,			// Control word này yêu cầu một tham số
	Skip,			// Skip qua các Control Word chưa update vào bảng TABLE_CTRLWORDS
	NF,				// Control word này thể hiện cho loại không có trong tài liệu RTF
};

const map<string, ControlType> TABLE_CTRLWORDS = {
{"'",				{ControlType::Symbol}},
{"-",				{ControlType::Symbol}},
{"*",				{ControlType::Symbol}},
{":",				{ControlType::Symbol}},
{"\\",				{ControlType::Symbol}},
{"_",				{ControlType::Symbol}},
{"{",				{ControlType::Symbol}},
{"|",				{ControlType::Symbol}},
{"}",				{ControlType::Symbol}},
{"~",				{ControlType::Symbol}},
{"rtf",				{ControlType::Destination}},
{"ansi",			{ControlType::Flag}},
{"ansicpg",			{ControlType::Value}},
{"deff",			{ControlType::Value}},
{"deftab",			{ControlType::Value}},
{"deflang",			{ControlType::Value}},
{"fonttbl",			{ControlType::Destination}},
{"f",				{ControlType::Value}},
{"colortbl",		{ControlType::Destination}},
{"red",				{ControlType::Value}},
{"green",			{ControlType::Value}},
{"blue",			{ControlType::Value}},
{"generator",		{ControlType::NF}},
{"fnil",			{ControlType::Flag}},
{"fcharset",		{ControlType::Value}},
{"viewkind",		{ControlType::Value}},
{"uc",				{ControlType::Value}},
{"stylesheet",		{ControlType::Destination}},
{"ql",				{ControlType::Flag}},
{"qr",				{ControlType::Flag}},
{"li",				{ControlType::Value}},
{"ri",				{ControlType::Value}},
{"sa",				{ControlType::Value}},
{"widctlpar",		{ControlType::Flag}},
{"wrapdefault",		{ControlType::Flag}},
{"aspalpha",		{ControlType::Toggle}},
{"aspnum",			{ControlType::Toggle}},
{"faauto",			{ControlType::Value}},
{"adjustright",		{ControlType::Value}},
{"rin",				{ControlType::Value}},
{"lin",				{ControlType::Value}},
{"itap",			{ControlType::Value}},
{"rtlch",			{ControlType::Flag}},
{"pard",			{ControlType::Flag}},
{"sl",				{ControlType::Value}},
{"slmult",			{ControlType::Value}},
{"cf",				{ControlType::Value}},
{"ul",				{ControlType::Toggle}},
{"b",				{ControlType::Toggle}},
{"fs",				{ControlType::Value}},
{"lang",			{ControlType::Value}},
{"par",				{ControlType::Symbol}},
{ "listtable",		{ControlType::Destination}}
};