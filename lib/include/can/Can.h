/*
 * This file is a part of the open source stm32plus library.
 * Copyright (c) 2011,2012,2013,2014 Andy Brown <www.andybrown.me.uk>
 * Please see website for licensing terms.
 */

#pragma once


namespace stm32plus {

  /**
   * Base class for the CAN peripheral
   */

  class Can {

    public:

      enum {
        E_TX_NO_MAILBOX = 1,
        E_STUFF_ERROR           = ((uint8_t)0x10),
        E_FORM_ERROR            = ((uint8_t)0x20),
        E_ACK_ERROR             = ((uint8_t)0x30),
        E_BIT_RECESSIVE_ERROR   = ((uint8_t)0x40),
        E_BID_DOMINANT_ERROR    = ((uint8_t)0x50),
        E_CRC_ERROR             = ((uint8_t)0x60),
        E_SOFTWARE_SET_ERROR    = ((uint8_t)0x70),
        
      };

    protected:
      CAN_TypeDef *_peripheralAddress;
      CAN_InitTypeDef _init;

    protected:
      Can(CAN_TypeDef *peripheralAddress);

    public:
      void sleep() const;
      void wakeup() const;

      // send remote frame

      uint8_t sendRemoteFrame(uint16_t StdId,uint8_t DLC) const;
      uint8_t sendRemoteFrame(uint16_t StdId,int32_t ExtId,uint8_t DLC) const;

      // send data frame

      uint8_t send(uint16_t StdId,uint8_t DLC,const void *data) const;
      uint8_t send(uint16_t StdId,int32_t ExtId,uint8_t DLC,const void *data) const;

      uint8_t send(CanTxMsg& msg) const;

      bool readyToReceive(uint8_t fifo) const;
      bool receive(uint8_t fifo, CanRxMsg* msg) const;

      bool transmissionPending (uint8_t mbox) const;
      bool hasErrors () const;
      
      operator CAN_TypeDef *();
      operator CAN_InitTypeDef *();
  };


  /**
   * Constructor
   * @param peripheralAddress the CAN base address
   */

  inline Can::Can(CAN_TypeDef *peripheralAddress)
    : _peripheralAddress(peripheralAddress) {
  }


  /**
   * Go to sleep
   */

  inline void Can::sleep() const {
    CAN_Sleep(_peripheralAddress);
  }


  /**
   * Wake up
   */

  inline void Can::wakeup() const {
    CAN_WakeUp(_peripheralAddress);
  }


  /**
   * Get the peripheral address
   * @return The CAN peripheral address
   */

  inline Can::operator CAN_TypeDef *() {
    return _peripheralAddress;
  }


  /**
   * Get the init structure
   * @return The CAN_InitTypeDef structure
   */

  inline Can::operator CAN_InitTypeDef *() {
    return &_init;
  }


  /**
   * Send a message
   * @param StdId
   * @param DLC
   * @return true if it worked
   */

  inline uint8_t Can::sendRemoteFrame(uint16_t StdId,uint8_t DLC) const {

    CanTxMsg msg;

    msg.IDE=CAN_Id_Standard;
    msg.RTR=CAN_RTR_Remote;
    msg.DLC=DLC;
    msg.StdId=StdId;

    return send(msg);
  }


  /**
   * Send a message
   * @param StdId
   * @param ExtId
   * @param DLC
   * @return true if it worked
   */

  inline uint8_t Can::sendRemoteFrame(uint16_t StdId,int32_t ExtId,uint8_t DLC) const {

    CanTxMsg msg;

    msg.IDE=CAN_Id_Extended;
    msg.RTR=CAN_RTR_Remote;
    msg.DLC=DLC;

    msg.StdId=StdId;
    msg.ExtId=ExtId;

    return send(msg);
  }


  /**
   * Send a message
   * @param StdId
   * @param DLC
   * @param data
   * @return true if it worked
   */

  inline uint8_t Can::send(uint16_t StdId,uint8_t DLC,const void *data) const {

    CanTxMsg msg;

    msg.IDE=CAN_Id_Standard;
    msg.RTR=CAN_RTR_Data;
    msg.DLC=DLC;
    msg.StdId=StdId;

    memcpy(msg.Data,data,DLC);

    return send(msg);
  }


  /**
   * Send a message
   * @param StdId
   * @param ExtId
   * @param DLC
   * @param data
   * @return true if it worked
   */

  inline uint8_t Can::send(uint16_t StdId,int32_t ExtId,uint8_t DLC,const void *data) const {

    CanTxMsg msg;

    msg.IDE=CAN_Id_Extended;
    msg.RTR=CAN_RTR_Data;
    msg.DLC=DLC;

    msg.StdId=StdId;
    msg.ExtId=ExtId;

    memcpy(msg.Data,data,DLC);

    return send(msg);
  }


  /**
   * Send a message given a message structure
   * @param msg
   * @return true if it worked
   */

  inline uint8_t Can::send(CanTxMsg& msg) const {
    
    auto mbox = CAN_Transmit(_peripheralAddress,&msg);
    if (mbox == CAN_TxStatus_NoMailBox)
      return errorProvider.set(ErrorProvider::ERROR_PROVIDER_CAN,E_TX_NO_MAILBOX,CAN_TxStatus_NoMailBox);

    return mbox;
  }


  /**
   * Check if the peripheral is ready to receive
   * @return true if it's ready
   */

  inline bool Can::readyToReceive(uint8_t fifo) const {
    return !!CAN_MessagePending(_peripheralAddress, fifo);
  }


  /**
   * Read a byte from the peripheral
   */

  inline bool Can::receive(uint8_t fifo,CanRxMsg* msg) const {

    if (!readyToReceive(fifo))
      return false;

    CAN_Receive(_peripheralAddress,fifo,msg);
    return true;
  }
                                                    
  inline bool Can::transmissionPending (uint8_t mbox) const {
      return CAN_TransmitStatus(_peripheralAddress, mbox) == CAN_TxStatus_Pending;
  }
  
  inline bool Can::hasErrors () const {
      auto ec = CAN_GetLastErrorCode(_peripheralAddress);
      if (ec != CAN_ErrorCode_NoErr)
      {
          return !errorProvider.set(ErrorProvider::ERROR_PROVIDER_CAN,ec,ec);
      }
      return false;
  }
}


