Scriptname ScaleformTestScript extends Quest

WEAPON Property myWeapon  Auto  

Event OnInit()
  Debug.Trace("Debug")
  ;UI.SetNumber("Magic Menu", "ValueName", 1)
  ;UI.SetNumber("Magic Menu", "a.ValueName", 1)
  ;UI.SetNumber("Magic Menu", "a.b.c.ValueName", 1)
  
  RegisterForUpdate(3);
  Game.GetPlayer().AddItem(myWeapon, 1, true)
EndEvent

Event OnUpdate()
	;UI.SetNumber("Magic Menu", "a.b.c.ValueName", 1)
EndEvent


