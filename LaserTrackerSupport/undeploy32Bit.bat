IF EXIST LMF.Tracker.Connection.tlb (
	C:\Windows\Microsoft.NET\Framework\v4.0.30319\regasm /u /tlb LMF.Tracker.Connection.dll
) ELSE (
	C:\Windows\Microsoft.NET\Framework\v4.0.30319\regasm /u LMF.Tracker.Connection.dll
)
