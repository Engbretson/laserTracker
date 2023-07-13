IF EXIST LMF.Tracker.Connection.tlb (
	C:\Windows\Microsoft.NET\Framework64\v4.0.30319\regasm /u /tlb LMF.Tracker.Connection.dll
	C:\Windows\Microsoft.NET\Framework64\v4.0.30319\regasm /codebase LMF.Tracker.Connection.dll /tlb
) ELSE (
	C:\Windows\Microsoft.NET\Framework64\v4.0.30319\regasm /u LMF.Tracker.Connection.dll
	C:\Windows\Microsoft.NET\Framework64\v4.0.30319\regasm /codebase LMF.Tracker.Connection.dll /tlb
)
