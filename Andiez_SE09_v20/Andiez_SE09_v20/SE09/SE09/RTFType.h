#pragma once
#include <Windows.h>
#include <iostream>
#include <ctype.h>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <list>
#include <map>

using namespace std;

#define keyword(name, num) (string(name) + to_string(num))

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
	CP_437,CP_708,CP_709,CP_710,CP_711,CP_720,CP_819,CP_850,CP_852,CP_860,CP_862,CP_863,CP_864,CP_865,CP_866,CP_932,CP_1250,CP_1251
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
	rdsSkip,
	rdsNorm
} RDS;

//RTF Internal State
typedef enum 	
{
	risHex,
	risBin,	
	risNorm,
	risPict
} RIS;

typedef enum
{
	sNorm,
	sSkip,
	dInfo,
	dParagraph,
	hFileTable,
	hListTable,
	hFontTable,
	hColorTable,
	hStyleSheet
} STATE;

typedef struct Save
{
	struct Save* pNext;		// Refers to the next storage.
	RDS rds;				// RTF Destination State
	RIS ris;				// RTF Internal State
} SAVE;

typedef struct Param
{
	int num;
	bool fParam;
} PARAM;

// This struct is list the RTF Control Word
typedef struct CtrlWord
{
	string key;
	int param;
	bool fParam;
	CtrlWord() : key("0"), param(0), fParam(false) {}
} CTRLWORD;

enum ControlType
{
	Flag,			// This control word will ignore any parameters.
	Destination,	// This control word marks the beginning of a group or destination. It disregards any parameters.
	Symbol,			// This control word represents a special character.
	Toggle,			// This control word is used to distinguish between the ON and OFF states (Not a parameter or parm != 0 => ON, param = 0 => OFF).
	Value,			// This control word requires a parameter.
	Skip,			// Skip control words that haven't been updated in the TABLE_CTRLWORDS table.
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
{"ansi",			{ControlType::Flag}},
{"ansicpg",			{ControlType::Value}},
{"deff",			{ControlType::Value}},
{"deftab",			{ControlType::Value}},
{"deflang",			{ControlType::Value}},
{"f",				{ControlType::Value}},
{"red",				{ControlType::Value}},
{"green",			{ControlType::Value}},
{"blue",			{ControlType::Value}},
{"generator",		{ControlType::Skip}},
{"fnil",			{ControlType::Flag}},
{"fcharset",		{ControlType::Value}},
{"viewkind",		{ControlType::Value}},
{"uc",				{ControlType::Value}},
{"ql",				{ControlType::Flag}},
{"qr",				{ControlType::Flag}},
{"li",				{ControlType::Value}},
{"ri",				{ControlType::Value}},
{"sa",				{ControlType::Value}},
{"widctlpar",		{ControlType::Flag}},
{"wrapdefault",		{ControlType::Flag}},
{"faauto",			{ControlType::Value}},
{"adjustright",		{ControlType::Value}},
{"rin",				{ControlType::Value}},
{"lin",				{ControlType::Value}},
{"itap",			{ControlType::Value}},
{"rtlch",			{ControlType::Flag}},
{"pard",			{ControlType::Flag}},
{"par",				{ControlType::Symbol}},
{"sl",				{ControlType::Value}},
{"slmult",			{ControlType::Value}},
{"cf",				{ControlType::Value}},
{"fs",				{ControlType::Value}},
{"lang",			{ControlType::Value}},
{"aspalpha",		{ControlType::Toggle}},
{"aspnum",			{ControlType::Toggle}},
{"ul",				{ControlType::Toggle}},
{"b",				{ControlType::Toggle}},
{"rtf",				{ControlType::Destination}},
{"fonttbl",			{ControlType::Destination}},
{"colortbl",		{ControlType::Destination}},
{"listtable",		{ControlType::Destination}},
{"info",			{ControlType::Destination}},
{"title",			{ControlType::Destination}},
{"subject",			{ControlType::Destination}},
{"author",			{ControlType::Destination}},
{"manager",			{ControlType::Destination}},
{"company",			{ControlType::Destination}},
{"operator",		{ControlType::Destination}},
{"category",		{ControlType::Destination}},
{"keywords",		{ControlType::Destination}},
{"comment",			{ControlType::Destination}},
{"revtim",			{ControlType::Destination}},
{"printim",			{ControlType::Destination}},
{"stylesheet",		{ControlType::Destination}},
{"creatim",			{ControlType::Destination}},
{"doccomm",			{ControlType::Destination}},
{"ftnsep",			{ControlType::Destination}},
{"ftncn",			{ControlType::Destination}},
{"header",			{ControlType::Destination}},
{"headerl",			{ControlType::Destination}},
{"headerr",			{ControlType::Destination}},
{"headerf",			{ControlType::Destination}},
{"footnote",		{ControlType::Destination}},
{"footer",			{ControlType::Destination}},
{"footerl",			{ControlType::Destination}},
{"footerr",			{ControlType::Destination}},
{"footerf",			{ControlType::Destination}},
{"pict",			{ControlType::Destination}},
{"xe",				{ControlType::Destination}},
{"rxe",				{ControlType::Destination}},
{"txe",				{ControlType::Destination}},
{"tc",				{ControlType::Destination}},
{"privatel",		{ControlType::Destination}},
};