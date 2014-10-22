Attribute VB_Name = "VJoy"
Public Type JoystickState
    ReportId As Byte
    Slider As Byte
    XAxis As Byte
    YAxis As Byte
    ZAxis As Byte
    XRotation As Byte
    YRotation As Byte
    ZRotation As Byte
    POV As Byte
    Buttons As Long
End Type

Public Declare Function VJoy_Initialize Lib "VJoy" (ByVal name As String, ByVal serial As String) As Integer
Public Declare Sub VJoy_Shutdown Lib "VJoy" ()
Public Declare Function VJoy_UpdateJoyState Lib "VJoy" (ByVal id As Integer, ByRef joyState As JoystickState) As Boolean

