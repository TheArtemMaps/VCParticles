�VCParticlesAPI    � 
 �                �  �
-M ����A      �  �
.M ����3FLYERS� �
	�  �M @���7FLYERS               ��L?     � �
{� M ����7	BOAT_WAKE               333?      ����FLAG   SRC �  {$CLEO .cs}
thread 'VCParticlesAPI'

while true
    wait 0
    10@ = Actor.CurrentCar($PLAYER_ACTOR)
    04C4: store_coords_to 1@ 2@ 3@ from_actor $PLAYER_ACTOR with_offset 0.0 0.0 0.0
   // 1337: add_particle "CARFLAME" x 0.0 y 0.0 z 8.5 xdir 0.0 ydir 0.0 zdir 0.0 size 0.8 r 0 g 0 b 0 a 0 lifespan 0
    //0107: 8@ = create_object #AD_FLATDOOR at 1@ 2@ 3@
  // 1346: stir_ground_around_object 8@ radius 1.0 density 5
     //1337: add_particle "FLYERS" x 0.0 y 0.0 z 1.5 xdir 0.0 ydir 0.0 zdir 0.0 size 0.8 r 0 g 0 b 0 a 0 lifespan 0
    if 
    is_key_pressed 45
    then
   //1350: add_particle_attached_to_a_vehicle "FLYERS" xoffset 0.0 yoffset 0.0 zoffset 1.5 xdir 0.0 ydir 0.0 zdir 0.0 size 0.8 r 0 g 0 b 0 a 0 lifespan 0 vehicle_handle 10@
    1341: add_big_gun_flash xfrom 1@ yfrom 2@ zfrom 3@ xto 1@ yto 2@ zto 3@
  //  1337: add_particle "SEX" x 1@ y 2@ z 3@ xdir 0.0 ydir 0.0 zdir 0.0 size 0.8 r 0 g 0 b 0 a 0 lifespan 0
  
   // 1342: add_moving_particle_effect "SEXPO" x 1@ y 2@ z 3@ xdir 0.0 ydir 0.0 zdir 0.5 scale 1.0 r 255 g 0 b 0 a 255 duration 1000 remove true
//    1342: add_moving_particle_effect "SMOKE_TRAIL" x 1@ y 2@ z 3@
  // 1346: stir_ground_around_object 8@ radius 1.0 density 5
  //  1338: add_particles_along_line "COCK" x1 1@ y1 2@ z1 3@ x2 1.0 y2 2.0 z2 3.0 x3 4.0 y3 5.0 z3 6.0 power 1.0 size 1.0 r 0 g 0 b 0 a 0 lifespan 0
    end
    if 
    is_key_pressed 46
    then
  //  0437: scatter_particle 23 0.3 at $4171 $4172 $4173 $4174 $4175 $4176
   // 1342: add_moving_particle_effect "SMOKE_TRAIL" 0.2 0 0 0 30000 at 1@ 2@ 3@ 0.05 0.05 0.05
  //  1345: remove_all_particle_objects
  // 1350: remove_particle_object "WATER_FOUNTAIN_VERT"
   // 1343: remove_particle_objects_in_area x1 1@ y1 2@ z1 3@ x2 1@ y2 2@ z1 3@
 //  1350: remove_particle_object 15@
    1333: remove_particle_system "FLYERS"
   // 1341: add_big_gun_flash xfrom 1@ yfrom 2@ zfrom 3@ xto 1@ yto 2@ zto 3@
   // 1335: add_jet_explosion x 1@ y 2@ z 3@ power 1.0 size 2.0 
    end
    if and
    is_key_pressed 9
    00DD:   actor $PLAYER_ACTOR driving_car_with_model #SKIMMER
    then
   // 7@ = Actor.CurrentCar($PLAYER_ACTOR)
   // 0407: create_coordinate 4@ 5@ 6@ from_car 10@ offset 0.0 -2.0 -2.0 
    1337: add_particle "FLYERS" x 4@ y 5@ z 6@ xdir 0.0 ydir 0.0 zdir 0.0 size 0.8 r 0 g 0 b 0 a 0 lifespan 0
//    1350: add_particle_attached_to_a_vehicle "FLYERS" xoffset 0.0 yoffset -2.0 zoffset -2.0 xdir 0.0 ydir 0.0 zdir 0.0 size 0.8 r 0 g 0 b 0 a 0 lifespan 0 vehicle_handle 10@
    end
    if and
    is_key_pressed 123
    04A7:   actor $PLAYER_ACTOR driving_boat
    then
  //   1350: add_particle_attached_to_a_vehicle "BOAT_WAKE" xoffset 0.0 yoffset -2.0 zoffset -2.0 xdir 0.0 ydir 0.0 zdir 0.0 size 0.8 r 0 g 0 b 0 a 0 lifespan 0 vehicle_handle 10@
    1337: add_particle "BOAT_WAKE" x 1@ y 2@ z 3@ xdir 0.0 ydir 0.0 zdir 0.0 size 0.7 r 0 g 0 b 0 a 0 lifespan 0
    end
 //      Actor.StorePos($PLAYER_ACTOR, 11@, 12@, 13@)
 //   0AE3: 8@ = random_object_near_point 11@ 12@ 13@ in_radius 30.0 find_next false //IF and SET
  //  1351: add_particle_attached_to_a_object "FLYERS" xoffset 0.0 yoffset 0.0 zoffset 1.0 xdir 0.0 ydir 0.0 zdir 0.0 size 0.8 r 0 g 0 b 0 a 0 lifespan 0 object_handle 8@
 //   Object.StorePos(8@, 14@, 15@, 16@)
 //   1349: add_yardie_door_smoke_at x 12@ y 13@ z 14@ attach_to_object 8@
 //   1348: start_boat_foam_animation
 //   1347: update_boat 8@ foam_animation
end
  __SBFTR 