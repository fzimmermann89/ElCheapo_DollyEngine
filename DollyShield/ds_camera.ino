/* 
 
 El Cheapo Dollyshield - Camera Control Functions
 modified Version of Dynamic Perception LLC's DollyShield ds_camera.ino
 (c) 2010-2011 C.A. Church / Dynamic Perception LLC
 (c) FFZ
 For more info go to http://openmoco.org or http://www.thundercorp.de/timelapse
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 */

/*

 ========================================
 Camera control functions
 ========================================
 
 */

void fire_camera(unsigned long exp_tm) {
  //Fires the camera by using the selected method.
 switch (shutter_mode){
  case SHUTTER_MODE_IR_NIKON:
   send_ir(IR_NIKON);
   break;
  case SHUTTER_MODE_IR_CANON:
   send_ir(IR_CANON);
   break;
  case SHUTTER_MODE_CABLE_FOCUS:
   digitalWriteFast(FOCUS_PIN, HIGH);
   //Fall through
  case SHUTTER_MODE_CABLE_NO_FOCUS:
   digitalWriteFast(CAMERA_PIN, HIGH);
   break;
 }
   S_CAM_ENGAGED=true; //Set Status flag

}


void stop_camera() {
 if ((shutter_mode==SHUTTER_MODE_IR_NIKON) && bulb_mode){
   //in Nikon IR-bulb mode send command again  
   send_ir(IR_NIKON);
   } 
 else if ((shutter_mode==SHUTTER_MODE_IR_CANON) && bulb_mode){
   //in Canon Nikon IR-bulb mode send command again
     send_ir(IR_CANON);
     }  
 else{
  //in both cable modes
  //we bring down both lines, just in case.. it doesnt hurt.
    digitalWriteFast(FOCUS_PIN, LOW);
    digitalWriteFast(CAMERA_PIN, LOW);
   }   

  //TODO
  // are we supposed to delay before allowing
  // the motors to move?  Register a timer
  // to clear out status flags, otherwise
  // just clear them out now.

  // the delay is used to prevent motor movement
  // when shot timing is controlled by the camera.
  // the post-delay should be set to an amount greater
  // than the max possible camera exposure timing

  // update camera currently engaged
 S_CAM_ENGAGED=false;

  // update camera cycle complete
 S_CAM_CYCLE_COMPLETE=true;

 }

void focus_camera(){
  if (shutter_mode==SHUTTER_MODE_CABLE_FOCUS){
    digitalWriteFast(FOCUS_PIN, HIGH);
    timer1_set(100,stop_cam_focus);
  }
  else S_IN_DELAY=false;
} 
void camera_clear() {
  // clears out camera engaged settings
  // so that motor control and other actions can 
  // be undertaken. 

 S_CAM_ENGAGED=false; 
 S_CAM_CYCLE_COMPLETE=true;

}  


void stop_cam_focus() {
  digitalWriteFast(FOCUS_PIN, LOW);
  //enable timer after Focus
  timer1_set(delay_focus,clear_cam_focus);
}

void clear_cam_focus() {
   //TODO: welches delay_status bit wann wo wie setzen?
  }

uint16_t calc_total_cam_tm() {
  // calculate total minimum time between exposures 
  // add 100ms pre-focus tap clear value
  uint16_t pf_tm = delay_focus>0?delay_focus + 100:0;
  //all the delays used in in mainhandler
  uint16_t total = (exp_tm + pf_tm + delay_postexp+ delay_preexp)+cam_repeat* (exp_tm + pf_tm + delay_repeat);

  if( m_mode==MODE_SMS ) total += m_sms_tm; //time used to drive the motors
  return(total);

}

uint16_t calc_min_cam_max(){
  uint16_t min_cam_max;
  min_cam_max=m_ramp_in+m_ramp_out;
  //if one of the ramps is enabled we need to more max shots than ramp shots
  if (min_cam_max>0) min_cam_max+=2;
  min_cam_max+=m_lead_in;
  min_cam_max+=m_lead_out;
  return min_cam_max;
}



void send_ir(uint8_t brand){
  //Sends IR sequence defined in seqs at position "brand".
  
  //go through the sequence. length is given in position 0.
  for(unsigned int i=1;i<=*(seqs[brand]);i++){
    int n=*(seqs[brand] + i);
    //make sure IR_PIN is low, necessary because of odd n
    digitalWriteFast(IR_PIN,LOW);
    while(n>0){
      n--;
      delayMicroseconds(OSCD);
      //toggle if in "on" cycle
      if (i%2)
      {digitalToggleFast(IR_PIN); 
      }  
    }
  }
}

