# $Id: user.spec,v 1.1 1994/02/24 08:05:33 hsu Exp $
#
name	user
id	2
length	540

1   pascal MessageBox(word ptr ptr word) MessageBox(1 2 3 4)
5   pascal InitApp(word) USER_InitApp(1)
6   pascal PostQuitMessage(word) PostQuitMessage(1)
10  pascal SetTimer(word word word ptr) SetTimer(1 2 3 4)
11  pascal SetSystemTimer(word word word ptr) SetSystemTimer(1 2 3 4)
12  pascal KillTimer(word word) KillTimer(1 2)
13  pascal GetTickCount() GetTickCount()
14  return GetTimerResolution 0 1000
15  pascal GetCurrentTime() GetTickCount()
16  pascal ClipCursor(ptr) ClipCursor(1)
17  pascal GetCursorPos(ptr) GetCursorPos(1)
18  pascal SetCapture(word) SetCapture(1)
19  pascal ReleaseCapture() ReleaseCapture()
20  pascal SetDoubleClickTime(word) SetDoubleClickTime(1)
21  pascal GetDoubleClickTime() GetDoubleClickTime()
22  pascal SetFocus(word) SetFocus(1)
23  pascal GetFocus() GetFocus()
28  pascal ClientToScreen(word ptr) ClientToScreen(1 2)
29  pascal ScreenToClient(word ptr) ScreenToClient(1 2)
30  pascal WindowFromPoint(long) WindowFromPoint(1)
31  pascal IsIconic(word) IsIconic(1)
32  pascal GetWindowRect(word ptr) GetWindowRect(1 2)
33  pascal GetClientRect(word ptr) GetClientRect(1 2)
34  pascal EnableWindow(word word) EnableWindow(1 2)
35  pascal IsWindowEnabled(word) IsWindowEnabled(1)
36  pascal GetWindowText(word ptr word) GetWindowText(1 2 3)
37  pascal SetWindowText(word ptr) SetWindowText(1 2)
38  pascal GetWindowTextLength(word) GetWindowTextLength(1)
39  pascal BeginPaint(word ptr) BeginPaint(1 2)
40  pascal EndPaint(word ptr) EndPaint(1 2)
41  pascal CreateWindow(ptr ptr long s_word s_word s_word s_word word word word ptr) 
	   CreateWindow(1 2 3 4 5 6 7 8 9 10 11)
42  pascal ShowWindow(word word) ShowWindow(1 2)
43  pascal CloseWindow(word) CloseWindow(1)
44  pascal OpenIcon(word) OpenIcon(1)
45  pascal BringWindowToTop(word) BringWindowToTop(1)
46  pascal GetParent(word) GetParent(1)
47  pascal IsWindow(word) IsWindow(1)
48  pascal IsChild(word word) IsChild(1 2)
49  pascal IsWindowVisible(word) IsWindowVisible(1)
50  pascal FindWindow(ptr ptr) FindWindow(1 2)
53  pascal DestroyWindow(word) DestroyWindow(1)
54  pascal EnumWindows(ptr long) EnumWindows(1 2)
55  pascal EnumChildWindows(word ptr long) EnumChildWindows(1 2 3)
56  pascal MoveWindow(word word word word word word) 
           MoveWindow(1 2 3 4 5 6)
57  pascal RegisterClass(ptr) RegisterClass(1)
58  pascal GetClassName(word ptr word) GetClassName(1 2 3)
59  pascal SetActiveWindow(word) SetActiveWindow(1)
60  pascal GetActiveWindow() GetActiveWindow()
61  pascal ScrollWindow(word s_word s_word ptr ptr) ScrollWindow(1 2 3 4 5)
62  pascal SetScrollPos(word word word word) SetScrollPos(1 2 3 4)
63  pascal GetScrollPos(word word) GetScrollPos(1 2)
64  pascal SetScrollRange(word word word word word) SetScrollRange(1 2 3 4 5)
65  pascal GetScrollRange(word word ptr ptr) GetScrollRange(1 2 3 4)
66  pascal GetDC(word) GetDC(1)
67  pascal GetWindowDC(word) GetWindowDC(1)
68  pascal ReleaseDC(word word) ReleaseDC(1 2)
69  pascal SetCursor(word) SetCursor(1)
70  pascal SetCursorPos(word word) SetCursorPos(1 2)
71  pascal ShowCursor(word) ShowCursor(1)
72  pascal SetRect(ptr s_word s_word s_word s_word) SetRect(1 2 3 4 5)
73  pascal SetRectEmpty(ptr) SetRectEmpty(1)
74  pascal CopyRect(ptr ptr) CopyRect(1 2)
75  pascal IsRectEmpty(ptr) IsRectEmpty(1)
76  pascal PtInRect(ptr long) PtInRect(1 2)
77  pascal OffsetRect(ptr s_word s_word) OffsetRect(1 2 3)
78  pascal InflateRect(ptr s_word s_word) InflateRect(1 2 3)
79  pascal IntersectRect(ptr ptr ptr) IntersectRect(1 2 3)
80  pascal UnionRect(ptr ptr ptr) UnionRect(1 2 3)
81  pascal FillRect(word ptr word) FillRect(1 2 3)
82  pascal InvertRect(word ptr) InvertRect(1 2)
83  pascal FrameRect(word ptr word) FrameRect(1 2 3)
84  pascal DrawIcon(word s_word s_word word) DrawIcon(1 2 3 4)
85  pascal DrawText(word ptr s_word ptr word) DrawText(1 2 3 4 5)
87  pascal DialogBox(word ptr word ptr) DialogBox(1 2 3 4)
88  pascal EndDialog(word s_word) EndDialog(1 2)
89  pascal CreateDialog(word ptr word ptr) CreateDialog(1 2 3 4)
90  pascal IsDialogMessage(word ptr) IsDialogMessage(1 2)
91  pascal GetDlgItem(word word) GetDlgItem(1 2)
92  pascal SetDlgItemText(word word ptr) SetDlgItemText(1 2 3)
93  pascal GetDlgItemText(word word ptr word) GetDlgItemText(1 2 3 4)
94  pascal SetDlgItemInt(word word word word) SetDlgItemInt(1 2 3 4)
95  pascal GetDlgItemInt(word word ptr word) GetDlgItemInt(1 2 3 4)
96  pascal CheckRadioButton(word word word word) CheckRadioButton(1 2 3 4)
97  pascal CheckDlgButton(word word word) CheckDlgButton(1 2 3)
98  pascal IsDlgButtonChecked(word word) IsDlgButtonChecked(1 2)
101 pascal SendDlgItemMessage(word word word word long)
	   SendDlgItemMessage(1 2 3 4 5)
102 pascal AdjustWindowRect(ptr long word) AdjustWindowRect(1 2 3)
103 pascal MapDialogRect(word ptr) MapDialogRect(1 2)
104 pascal MessageBeep(word) MessageBeep(1)
106 pascal GetKeyState(word) GetKeyState(1)
107 pascal DefWindowProc(word word word long) DefWindowProc(1 2 3 4)
108 pascal GetMessage(ptr word word word) GetMessage(1 2 3 4)
109 pascal PeekMessage(ptr word word word word) PeekMessage(1 2 3 4 5)
110 pascal PostMessage(word word word long) PostMessage(1 2 3 4)
111 pascal SendMessage(word word word long) SendMessage(1 2 3 4)
113 pascal TranslateMessage(ptr) TranslateMessage(1)
114 pascal DispatchMessage(ptr) DispatchMessage(1)
118 pascal RegisterWindowMessage(ptr) RegisterWindowMessage(1)
119 pascal GetMessagePos() GetMessagePos()
120 pascal GetMessageTime() GetMessageTime()
122 pascal CallWindowProc(ptr word word word long) CallWindowProc(1 2 3 4 5)
124 pascal UpdateWindow(word) UpdateWindow(1)
125 pascal InvalidateRect(word ptr word) InvalidateRect(1 2 3)
126 pascal InvalidateRgn(word word word) InvalidateRgn(1 2 3)
127 pascal ValidateRect(word ptr) ValidateRect(1 2)
128 pascal ValidateRgn(word word) ValidateRgn(1 2)
129 pascal GetClassWord(word s_word) GetClassWord(1 2)
130 pascal SetClassWord(word s_word word) SetClassWord(1 2 3)
131 pascal GetClassLong(word s_word) GetClassLong(1 2)
132 pascal SetClassLong(word s_word long) SetClassLong(1 2 3)
133 pascal GetWindowWord(word s_word) GetWindowWord(1 2)
134 pascal SetWindowWord(word s_word word) SetWindowWord(1 2 3)
135 pascal GetWindowLong(word s_word) GetWindowLong(1 2)
136 pascal SetWindowLong(word s_word long) SetWindowLong(1 2 3)
150 pascal LoadMenu(word ptr) LoadMenu(1 2)
151 pascal CreateMenu() CreateMenu()
152 pascal DestroyMenu(word) DestroyMenu(1)
153 pascal ChangeMenu(word word ptr word word) ChangeMenu(1 2 3 4 5)
154 pascal CheckMenuItem(word word word) CheckMenuItem(1 2 3)
155 pascal EnableMenuItem(word word word) EnableMenuItem(1 2 3)
156 pascal GetSystemMenu(word word) GetSystemMenu(1 2)
157 pascal GetMenu(word) GetMenu(1)
158 pascal SetMenu(word word) SetMenu(1 2)
159 pascal GetSubMenu(word word) GetSubMenu(1 2)
160 pascal DrawMenuBar(word) DrawMenuBar(1)
163 pascal CreateCaret(word word word word) CreateCaret(1 2 3 4)
164 pascal DestroyCaret() DestroyCaret()
165 pascal SetCaretPos(word word) SetCaretPos(1 2)
166 pascal HideCaret(word) HideCaret(1)
167 pascal ShowCaret(word) ShowCaret(1)
166 pascal SetCaretBlinkTime(word) SetCaretBlinkTime(1)
169 pascal GetCaretBlinkTime() GetCaretBlinkTime()
171 pascal WinHelp(word word long) WinHelp(1 2 3)
173 pascal LoadCursor(word ptr) LoadCursor(1 2)
174 pascal LoadIcon(word ptr) LoadIcon(1 2)
175 pascal LoadBitmap(word ptr) LoadBitmap(1 2)
176 pascal LoadString(word word ptr s_word) LoadString(1 2 3 4)
177 pascal LoadAccelerators(word ptr) LoadAccelerators(1 2)
178 pascal TranslateAccelerator(word word ptr) TranslateAccelerator(1 2 3)
179 pascal GetSystemMetrics(word) GetSystemMetrics(1)
180 pascal GetSysColor(word) GetSysColor(1)
181 pascal SetSysColors(word ptr ptr) SetSysColors(1 2 3)
182 pascal KillSystemTimer(word word) KillSystemTimer(1 2)
183 pascal GetCaretPos(ptr) GetCaretPos(1)
190 pascal GetUpdateRect(word ptr word) GetUpdateRect(1 2 3)
191 pascal ChildWindowFromPoint(word long) ChildWindowFromPoint(1 2)
200 pascal OpenComm(ptr word word) OpenComm(1 2 3)
201 pascal SetCommState(ptr) SetCommState(1)
202 pascal GetCommState(word ptr) GetCommState(1 2)
203 pascal GetCommError(word ptr) GetCommError(1 2)
204 pascal ReadComm(word ptr word) ReadComm(1 2 3)
205 pascal WriteComm(word ptr word) WriteComm(1 2 3)
206 pascal TransmitCommChar(word byte) TransmitCommChar (1 2)
207 pascal CloseComm(word) CloseComm(1)
208 pascal SetCommEventMask(word word) SetCommEventMask(1 2)
209 pascal GetCommEventMask(word word) GetCommEventMask(1 2)
210 pascal SetCommBreak(word) SetCommBreak(1)
211 pascal ClearCommBreak(word) ClearCommBreak(1)
212 pascal UngetCommChar(word byte) UngetCommChar(1 2)
213 pascal BuildCommDCB(ptr ptr) BuildCommDCB(1 2)
214 pascal EscapeCommFunction(word word) EscapeCommFunction(1 2)
215 pascal FlushComm(word word) FlushComm(1 2)
218 pascal DialogBoxIndirect(word word word ptr) DialogBoxIndirect(1 2 3 4)
219 pascal CreateDialogIndirect(word ptr word ptr)
	   CreateDialogIndirect(1 2 3 4)
221 pascal ScrollDC(word s_word s_word ptr ptr word ptr) 
	   ScrollDC(1 2 3 4 5 6 7)
227 pascal GetNextDlgGroupItem(word word word) GetNextDlgGroupItem(1 2 3)
228 pascal GetNextDlgTabItem(word word word) GetNextDlgTabItem(1 2 3)
229 pascal GetTopWindow(word) GetTopWindow(1)
230 pascal GetNextWindow(word word) GetNextWindow(1 2)
232 pascal SetWindowPos(word word word word word word word) 
           SetWindowPos(1 2 3 4 5 6 7)
236 pascal GetCapture() GetCapture()
237 pascal GetUpdateRgn(word word word) GetUpdateRgn(1 2 3)
239 pascal DialogBoxParam(word ptr word ptr long) DialogBoxParam(1 2 3 4 5)
240 pascal DialogBoxIndirectParam(word word word ptr long)
	   DialogBoxIndirectParam(1 2 3 4 5)
241 pascal CreateDialogParam(word ptr word ptr long)
	   CreateDialogParam(1 2 3 4 5)
242 pascal CreateDialogIndirectParam(word ptr word ptr long)
	   CreateDialogIndirectParam(1 2 3 4 5)
244 pascal EqualRect(ptr ptr) EqualRect(1 2)
258 pascal MapWindowPoints(word word ptr word) MapWindowPoints(1 2 3 4)
262 pascal GetWindow(word word) GetWindow(1 2)
266 pascal SetMessageQueue(word) SetMessageQueue(1)
267 pascal ShowScrollBar(word word word) ShowScrollBar(1 2 3)
272 pascal IsZoomed(word) IsZoomed(1)
277 pascal GetDlgCtrlID(word) GetDlgCtrlID(1)
282 pascal SelectPalette(word word word) SelectPalette(1 2 3)
283 pascal RealizePalette(word) RealizePalette(1)
286 pascal GetDesktopWindow() GetDesktopWindow()
288 pascal GetMessageExtraInfo() GetMessageExtraInfo()
290 pascal RedrawWindow(word ptr word word) RedrawWindow(1 2 3 4)
319 pascal ScrollWindowEx(word s_word s_word ptr ptr word ptr word) 
	   ScrollWindowEx(1 2 3 4 5 6 7 8)
324 pascal FillWindow(word word word word) FillWindow(1 2 3 4)
325 pascal PaintRect(word word word word ptr) PaintRect(1 2 3 4 5)
334 pascal GetQueueStatus(word) GetQueueStatus(1)
335 pascal GetInputState() GetInputState()
359 pascal GetDCEx(word word long) GetDCEx(1 2 3)
370 pascal GetWindowPlacement(word ptr) GetWindowPlacement(1 2)
371 pascal SetWindowPlacement(word ptr) SetWindowPlacement(1 2)
373 pascal SubtractRect(ptr ptr ptr) SubtractRect(1 2 3)
403 pascal UnregisterClass(ptr word) UnregisterClass(1 2)
404 pascal GetClassInfo(word ptr ptr) GetClassInfo(1 2 3)
406 pascal CreateCursor(word word word word word ptr ptr) 
	CreateCursor(1 2 3 4 5 6 7)
410 pascal InsertMenu(word word word word ptr) InsertMenu(1 2 3 4 5)
411 pascal AppendMenu(word word word ptr) AppendMenu(1 2 3 4)
412 pascal RemoveMenu(word word word) RemoveMenu(1 2 3)
413 pascal DeleteMenu(word word word) DeleteMenu(1 2 3)
414 pascal ModifyMenu(word word word word ptr) ModifyMenu(1 2 3 4 5)
415 pascal CreatePopupMenu() CreatePopupMenu()
416 pascal TrackPopupMenu(word word word word word word ptr) 
	   TrackPopupMenu(1 2 3 4 5 6 7)
418 pascal SetMenuItemBitmaps(word word word word word) 
	   SetMenuItemBitmaps(1 2 3 4 5)
420 pascal wsprintf(ptr ptr) wsprintf(1 2)
421 pascal wvsprintf(ptr ptr ptr) wvsprintf(1 2 3)
430 pascal lstrcmp(ptr ptr) lstrcmp(1 2)
431 pascal AnsiUpper(ptr) AnsiUpper(1)
432 pascal AnsiLower(ptr) AnsiLower(1)
433 pascal IsCharAlpha(byte) IsCharAlpha(1)
434 pascal IsCharAlphanumeric(byte) IsCharAlphanumeric(1)
435 pascal IsCharUpper(byte) IsCharUpper(1)
436 pascal IsCharLower(byte) IsCharLower(1)
437 pascal AnsiUpperBuff(ptr word) AnsiUpperBuff(1 2)
438 pascal AnsiLowerBuff(ptr word) AnsiLowerBuff(1 2)
452 pascal CreateWindowEx(long ptr ptr long s_word s_word s_word s_word 
	                  word word word ptr) 
	   CreateWindowEx(1 2 3 4 5 6 7 8 9 10 11 12)
454 pascal AdjustWindowRectEx(ptr long word long) AdjustWindowRectEx(1 2 3 4)
457 pascal DestroyIcon(word) DestroyIcon(1)
458 pascal DestroyCursor(word) DestroyCursor(1)
460 pascal GetInternalWindowPos(word ptr ptr) GetInternalWindowPos(1 2 3)
461 pascal SetInternalWindowPos(word word ptr ptr) SetInternalWindowPos(1 2 3 4)
466 pascal DrawFocusRect(word ptr) DrawFocusRect(1 2)
471 pascal lstrcmpi(ptr ptr) lstrcmpi(1 2)
472 pascal AnsiNext(ptr) AnsiNext(1 )
473 pascal AnsiPrev(ptr ptr) AnsiPrev(1 2)
