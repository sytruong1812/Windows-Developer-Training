; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CAbstract
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "EC1Assignment.h"

ClassCount=4
Class1=CEC1AssignmentApp
Class2=CEC1AssignmentDlg
Class3=CAboutDlg

ResourceCount=4
Resource1=IDD_EC1ASSIGNMENT_DIALOG
Resource2=IDR_MAINFRAME
Resource3=IDD_ABOUTBOX
Class4=CAbstract
Resource4=IDD_ABSTRACT

[CLS:CEC1AssignmentApp]
Type=0
HeaderFile=EC1Assignment.h
ImplementationFile=EC1Assignment.cpp
Filter=N

[CLS:CEC1AssignmentDlg]
Type=0
HeaderFile=EC1AssignmentDlg.h
ImplementationFile=EC1AssignmentDlg.cpp
Filter=D
LastObject=CEC1AssignmentDlg
BaseClass=CDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=EC1AssignmentDlg.h
ImplementationFile=EC1AssignmentDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=7
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308480
Control6=IDC_STATIC,static,1342308480
Control7=IDC_STATIC,static,1342308352

[DLG:IDD_EC1ASSIGNMENT_DIALOG]
Type=1
Class=CEC1AssignmentDlg
ControlCount=20
Control1=IDCANCEL,button,1342242816
Control2=IDC_NOTES,edit,1353777284
Control3=IDC_DECRYPT,button,1342242816
Control4=IDC_ENCRYPT,button,1342242816
Control5=IDC_GEN_KEY,button,1342242816
Control6=IDC_PRIVATE_KEY,edit,1350631552
Control7=IDC_STATIC,button,1342177287
Control8=IDC_PUBLIC_KEY,edit,1350631552
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_EMPTY_NOTES,button,1342242816
Control13=IDC_VALUES,edit,1353777284
Control14=IDC_PRIME1,edit,1350631552
Control15=IDC_STATIC,button,1342178055
Control16=IDC_CHECK_P1,button,1342242819
Control17=IDC_PRIME2,edit,1350631552
Control18=IDC_CHECK_P2,button,1342242819
Control19=IDC_COMPARE,button,1342242816
Control20=IDC_ABSTRACT,button,1342242816

[DLG:IDD_ABSTRACT]
Type=1
Class=CAbstract
ControlCount=2
Control1=IDC_CLOSE,button,1342242816
Control2=IDC_TEXT,RICHEDIT,1352732740

[CLS:CAbstract]
Type=0
HeaderFile=Abstract.h
ImplementationFile=Abstract.cpp
BaseClass=CDialog
Filter=D
LastObject=CAbstract
VirtualFilter=dWC

