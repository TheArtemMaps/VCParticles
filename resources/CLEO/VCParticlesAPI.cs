�VCParticlesAPI    �                �  �
-M P���7FLYERS                        BROCKET_TRAIL              ?  �?�   � (#�  �
.M 7���E3TEST� �
	�  �M �����          �   �   7FLYERS               ��L?     �  �
{M �����  	 
  �
	 
     A HG  ����FLAG   SRC   {$CLEO .cs}
thread 'VCParticlesAPI'
while true
    wait 0
    04C4: store_coords_to 1@ 2@ 3@ from_actor $PLAYER_ACTOR with_offset 0.0 0.0 0.0
    //0107: 8@ = create_object #AD_FLATDOOR at 1@ 2@ 3@
  // 1346: stir_ground_around_object 8@ radius 1.0 density 5
    if 
    is_key_pressed 45
    then
  // 0107: 8@ = create_object #AD_FLATDOOR at 1@ 2@ 3@
    1337: add_particle "FLYERS" x 1@ y 2@ z 3@ xdir 0.0 ydir 0.0 zdir 0.0 size 0.0 r 0 g 0 b 0 a 0 lifespan 0
    1342: add_moving_particle_effect "ROCKET_TRAIL" x 1@ y 2@ z 3@ xdir 0.0 ydir 0.0 zdir 0.5 scale 1.0 r 255 g 0 b 0 a 255 duration 9000 remove true
//    1342: add_moving_particle_effect "SMOKE_TRAIL" x 1@ y 2@ z 3@
  // 1346: stir_ground_around_object 8@ radius 1.0 density 5
   // 1338: add_particles_along_line "RAINDROP" x1 1@ y1 2@ z1 3@ x2 1.0 y2 2.0 z2 3.0 x3 4.0 y3 5.0 z3 6.0 power 1.0 size 1.0 r 0 g 0 b 0 a 0 lifespan 0
    end
    if 
    is_key_pressed 46
    then
  //  0437: scatter_particle 23 0.3 at $4171 $4172 $4173 $4174 $4175 $4176
   // 1342: add_moving_particle_effect "SMOKE_TRAIL" 0.2 0 0 0 30000 at 1@ 2@ 3@ 0.05 0.05 0.05
    1345: remove_all_particle_objects
   // 1343: remove_particle_objects_in_area x1 1@ y1 2@ z1 3@ x2 1@ y2 2@ z1 3@
    1333: remove_particle_system "TEST"
   // 1341: add_big_gun_flash xfrom 1@ yfrom 2@ zfrom 3@ xto 1@ yto 2@ zto 3@
   // 1335: add_jet_explosion x 1@ y 2@ z 3@ power 1.0 size 2.0 
    end
    if and
    is_key_pressed 9
    00DD:   actor $PLAYER_ACTOR driving_car_with_model #SKIMMER
    then
    7@ = Actor.CurrentCar($PLAYER_ACTOR)
    0407: create_coordinate 4@ 5@ 6@ from_car 7@ offset 0.0 -2.0 -2.0 
    1337: add_particle "FLYERS" x 4@ y 5@ z 6@ xdir 0.0 ydir 0.0 zdir 0.0 size 0.8 r 0 g 0 b 0 a 0 lifespan 0
    end
    if 
    is_key_pressed 123
    then
    Actor.StorePos($PLAYER_ACTOR, 9@, 10@, 11@)
    0AE3: 8@ = random_object_near_point 9@ 10@ 11@ in_radius 10.0 find_next 1 //IF and SET
    1348: start_boat_foam_animation
    1347: update_boat 8@ foam_animation
    end
end
v  __SBFTR 