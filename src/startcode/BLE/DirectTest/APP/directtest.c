/********************************** (C) COPYRIGHT *******************************
* File Name          : directtest.c
* Author             : WCH
* Version            : V1.0
* Date               : 2018/11/12
* Description        : ֱ�Ӳ��Գ��򣬲���ָ��ͨ��Ƶ���������ݰ�
*******************************************************************************/




/******************************************************************************/
/* ͷ�ļ����� */
#include "CONFIG.h"
#include "CH57x_common.h"
#include "HAL.h"
#include "directtest.h"


static tmosTaskID testTaskID;
#if BLE_DIRECT_TEST
static u8 TestEnalbe = FALSE;
#endif


/*******************************************************************************
 * @fn          HAL_ProcessEvent
 *
 * @brief       Ӳ����������
 *
 * input parameters
 *
 * @param       task_id.
 * @param       events.
 *
 * output parameters
 *
 * @param       events.
 *
 * @return      None.
 */
tmosEvents TEST_ProcessEvent( tmosTaskID task_id, tmosEvents events )
{
  static u8 payload=0;
	uint8 * msgPtr;
  keyChange_t *msgKeyPtr;

  if( events & SYS_EVENT_MSG ){  // ����HAL����Ϣ������tmos_msg_receive��ȡ��Ϣ��������ɺ�ɾ����Ϣ��
		msgPtr = tmos_msg_receive(task_id);
    if( msgPtr ){
			switch(  ((tmos_event_hdr_t*)msgPtr)->event){
        case KEY_CHANGE:
          msgKeyPtr = (keyChange_t *)msgPtr;
          if( msgKeyPtr->keys&HAL_KEY_SW_1 ){
            if( TestEnalbe == FALSE ){ 
              payload++;
              TestEnalbe = TRUE;
              HalLedBlink( HAL_LED_1, 0xff, 30 , 4000);
              API_LE_TransmitterTestCmd( 0, 20, payload&7, 0x15 );
              PRINT("(key)test start ...\n");
            }
            else{
              TestEnalbe = FALSE;
              HalLedSet( HAL_LED_1, HAL_LED_MODE_OFF );
              API_LE_TestEndCmd( );
              PRINT("   (key)end!\n");
            }
            tmos_stop_task( testTaskID, TEST_EVENT );
          }
          break;
        default:
          break;
      }
      /* De-allocate */
      tmos_msg_deallocate( msgPtr );
    }
    return events ^ SYS_EVENT_MSG;
	}
  if( events & TEST_EVENT ){
    if( TestEnalbe == FALSE ){ 
      payload++;
      TestEnalbe = TRUE;
      HalLedBlink( HAL_LED_1, 0xff, 30 , 4000);
      API_LE_TransmitterTestCmd( 0, 20, payload&7, 0x15 );
      tmos_start_task( testTaskID , TEST_EVENT ,MS1_TO_SYSTEM_TIME(20*1000) ); // ����ʱ��20s
      PRINT("test start ...\n");
    }
    else{
      TestEnalbe = FALSE;
      HalLedSet( HAL_LED_1, HAL_LED_MODE_OFF );
      API_LE_TestEndCmd();
      PRINT("   end!\n");
    }
		return events ^ TEST_EVENT;
  }
  return 0;
}

/*******************************************************************************
 * @fn          Hal_Init
 *
 * @brief       Ӳ����ʼ��
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
 void TEST_Init( )
{
  testTaskID = TMOS_ProcessEventRegister(TEST_ProcessEvent);
#if (defined HAL_KEY) && (HAL_KEY == TRUE)
  HAL_KEY_RegisterForKeys(testTaskID);
#endif
#if BLE_DIRECT_TEST
  tmos_start_task( testTaskID , TEST_EVENT ,1000 ); // ����һ����������
#endif
}
/******************************** endfile @ mcu ******************************/