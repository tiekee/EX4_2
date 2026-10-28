import unreal

tools=unreal.AssetToolsHelpers.get_asset_tools()
base='/Game/CH4_2'
def blueprint(name,parent):
    path=base+'/'+name
    asset=unreal.load_asset(path)
    if not asset:
        factory=unreal.BlueprintFactory()
        factory.set_editor_property('parent_class',parent)
        asset=tools.create_asset(name,base,unreal.Blueprint,factory)
    unreal.BlueprintEditorLibrary.compile_blueprint(asset)
    return asset,unreal.load_class(None,path+'.'+name+'_C')

coin,coin_class=blueprint('BP_Coin',unreal.CXItem)
buff,buff_class=blueprint('BP_SpeedBuff',unreal.CXSpeedBuff)
spawner,spawner_class=blueprint('BP_ItemSpawner',unreal.CXItemSpawner)
ps,ps_class=blueprint('BP_PlayerState',unreal.CXPlayerState)
gs,gs_class=blueprint('BP_GameState',unreal.CXGameState)
widget_path=base+'/WBP_MatchUI'
widget=unreal.load_asset(widget_path)
if not widget:
    factory=unreal.WidgetBlueprintFactory()
    factory.set_editor_property('parent_class',unreal.CXMatchUI)
    widget=tools.create_asset('WBP_MatchUI',base,unreal.WidgetBlueprint,factory)
unreal.BlueprintEditorLibrary.compile_blueprint(widget)
widget_class=unreal.load_class(None,widget_path+'.WBP_MatchUI_C')
pc_class=unreal.load_class(None,'/Game/VehicleTemplate/Blueprints/BP_VehicleAdvPlayerController.BP_VehicleAdvPlayerController_C')
gm_class=unreal.load_class(None,'/Game/VehicleTemplate/Blueprints/BP_VehicleAdvGameMode.BP_VehicleAdvGameMode_C')
car_class=unreal.load_class(None,'/Game/VehicleTemplate/Blueprints/SportsCar/BP_SportsCar_Pawn.BP_SportsCar_Pawn_C')
pc=unreal.get_default_object(pc_class)
pc.set_editor_property('match_ui_class',widget_class)
gm=unreal.get_default_object(gm_class)
gm.set_editor_property('player_state_class',ps_class)
gm.set_editor_property('game_state_class',gs_class)
gm.set_editor_property('waiting_duration',10)
gm.set_editor_property('playing_duration',120)
action=unreal.load_asset(base+'/IA_Attack')
if not action:
    action=tools.create_asset('IA_Attack',base,unreal.InputAction,unreal.InputAction_Factory())
action.set_editor_property('value_type',unreal.InputActionValueType.BOOLEAN)
context=unreal.load_asset('/Game/VehicleTemplate/Input/IMC_Default')
context.unmap_all_keys_from_action(action)
key=unreal.Key()
key.set_editor_property('key_name','LeftMouseButton')
context.map_key(action,key)
unreal.get_default_object(car_class).set_editor_property('attack_action',action)
unreal.get_default_object(car_class).set_editor_property('replicates',True)
for path in ['/Game/VehicleTemplate/Blueprints/BP_VehicleAdvPlayerController','/Game/VehicleTemplate/Blueprints/BP_VehicleAdvGameMode','/Game/VehicleTemplate/Blueprints/SportsCar/BP_SportsCar_Pawn']:
    unreal.BlueprintEditorLibrary.compile_blueprint(unreal.load_asset(path))

levels=unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
levels.load_level('/Game/VehicleTemplate/Maps/VehicleAdvExampleMap')
actors=unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
world=unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
world.get_world_settings().set_editor_property('default_game_mode',gm_class)
starts=[a for a in actors.get_all_level_actors() if isinstance(a,unreal.PlayerStart)]
for a in starts:
    unreal.log('CX_START '+str(a.get_actor_location())+' '+str(a.get_actor_rotation()))
if not starts:
    starts=[actors.spawn_actor_from_class(unreal.PlayerStart,unreal.Vector(0,0,150))]
origin=starts[0].get_actor_location()
rotation=starts[0].get_actor_rotation()
while len(starts)<3:
    loc=origin+unreal.Vector(0,len(starts)*650,0)
    starts.append(actors.spawn_actor_from_class(unreal.PlayerStart,loc,rotation))
existing=[a for a in actors.get_all_level_actors() if isinstance(a,unreal.CXItemSpawner)]
if not existing:
    for i in range(5):
        for j in range(4):
            loc=origin+unreal.Vector(800+i*700,(j-1)*650,-20)
            actor=actors.spawn_actor_from_class(spawner_class,loc)
            actor.set_actor_label(('Buff' if (i+j)%4==0 else 'Coin')+'_Spawner_'+str(i)+'_'+str(j))
            actor.set_editor_property('item_class',buff_class if (i+j)%4==0 else coin_class)
levels.save_current_level()
unreal.EditorAssetLibrary.save_directory('/Game/CH4_2',False,True)
unreal.EditorAssetLibrary.save_directory('/Game/VehicleTemplate',False,True)
unreal.log('CX_PROJECT_CONFIGURED')

