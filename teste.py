
# -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#  PCMPy - Lib
#  Python Extension for  Oracle BRM
#  Copyright Cassio Jemael 2024
# -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= 


import PCMPy

#print(PCMPy.pcm_connect())


flistin = "0 PIN_FLD_POID       POID [0] 0.0.0.1 /invoice 226060 0"

custom_fld = "C_FLD_CHARGE_PENALTY_EXEMPT_FLAG"
custom_opcode = "C_OP_COLLECTIONS_POL_EXIT_CREDIT_INSTALLMENT"

update_register = "0 PIN_FLD_POID       POID [0] 0.0.0.1 /account 159593 99 \n\
0 PIN_FLD_EFFECTIVE_T      TSTAMP [0] (1751515434))"



data = {
    "PIN_FLD_POID": "0.0.0.1 /dd/fields -1 0",       
    "PIN_FLD_FIELD": [
        {
            "PIN_FLD_FIELD_NAME": "PIN_FLD_DUE_T",
            "PIN_FLD_FIELD_NUM": 0
        }
    ]
}





# Exemplo de acesso aos dados
#print(data["PIN_FLD_POID"])
#print(data["PIN_FLD_FIELD"][0]["PIN_FLD_FIELD_NAME"])




print("Initializating PCMPy Extension tests")




try:
    print("Conecting CM")
    if ( PCMPy.pcm_connect() ) :
        print("OK")
    else :
        print("Error")
except PCMPy.error as e:
    print("Error pcm_connect: " , e)


try:
    print("Changing Context and conecting CM")
    if ( PCMPy.pcm_set_context(1)) :
        print("OK")
    else :
        print("Error")
except PCMPy.error as e:
    print("Error pcm_set_context: " , e)

try:
    print("Returning Context and reconecting CM")
    if ( PCMPy.pcm_set_context(0) >= 0) :
        print("OK")
    else :
        print("Error")
except PCMPy.error as e:
    print("Error pcm_set_context: " , e)


try:
    print("Executing string flist")
    retorno = PCMPy.pcm_op(PCMPy.PCM_OP_READ_OBJ, flistin, PCMPy.PCM_OPFLG_NO_DESCEND)
    if ( retorno ) :
        print("OK")
        print(retorno)
    else :
        print("Error")
except PCMPy.error as e:
    print("Error pcm_op: " , e)    



try:
    print("Executing array flist")
    retorno = PCMPy.pcm_op(PCMPy.PCM_OP_SDK_GET_FLD_SPECS, data, 0)
    if ( retorno ) :
        print("OK")
        print(retorno)
    else :
        print("Error")
except PCMPy.error as e:
    print("Error pcm_op: " , e)    



try:
    print("Testing poid Creation")    
    poid_teste = PCMPy.pcm_create_poid(393217, "/bill", 63325623 )
    if ( poid_teste ) :
        print("OK")
        print(poid_teste)
    else :
        print("Error")
except PCMPy.error as e:
    print("Error pcm_create_poid: " , e)    


try:
    print("Testing get db from poid")    
    retorno = PCMPy.pcm_get_poid_db(poid_teste)
    if ( retorno ) :
        print("OK")
        print(retorno)
    else :
        print("Error")
except PCMPy.error as e:
    print("Error pcm_get_poid_db: " , e)        

try:
    print("Testing get type from poid")    
    retorno = PCMPy.pcm_get_poid_type(poid_teste)
    if ( retorno ) :
        print("OK")
        print(retorno)
    else :
        print("Error")
except PCMPy.error as e:
    print("Error pcm_get_poid_type: " , e)        

try:
    print("Testing get number from poid")    
    retorno = PCMPy.pcm_get_poid_number(poid_teste)
    if ( retorno ) :
        print("OK")
        print(retorno)
    else :
        print("Error")
except PCMPy.error as e:
    print("Error pcm_get_poid_number: " , e)        

try:
    print("Testing search field spec")    
    retorno = PCMPy.pcm_get_field_spec(custom_fld)    
    if ( retorno ) :        
        #print(retorno)
        print( retorno['type'] )
        print( retorno['number'] )
        print("OK")
    else :
        print("Error")
except PCMPy.error as e:
    print("Error pcm_get_poid_number: " , e)        


try:
    print("Testing search custom field spec")    
    retorno = PCMPy.pcm_get_field_spec('PIN_FLD_DUE_T')    
    if ( retorno ) :
        print(retorno)
        print("OK")
    else :
        print("Error")
except PCMPy.error as e:
    print("Error pcm_get_poid_number: " , e)        


try:
    print("Testing search custom opcode")    
    retorno = PCMPy.pcm_get_custom_opcode(custom_opcode)    
    if ( retorno >=0) :
        print(retorno)
        print("OK")
    else :
        print("Error")
except PCMPy.error as e:
    print("Error pcm_get_custom_opcode: " , e)        


try:
    print("Testing Open Transactions")    
    
    if ( PCMPy.pcm_open_transaction(PCMPy.PCM_TRANS_OPEN_READWRITE) >= 0 ) :        
        print("OK")
    else :
        print("Error")
except PCMPy.error as e:
    print("Error pcm_open_transaction: " , e)        


try:
    print("Testing Update register")    
    
    retorno = PCMPy.pcm_op(PCMPy.PCM_OP_WRITE_FLDS, update_register, 0)
    if ( retorno ) :
        print("OK")
        print(retorno)
        print("OK")
    else :
        print("Error")
except PCMPy.error as e:
    print("Error pcm_op: " , e)        


try:
    print("Testing Abort Transactions")    
    
    if ( PCMPy.pcm_abort_transaction() >= 0 ) :        
        print("OK")
    else :
        print("Error")
except PCMPy.error as e:
    print("Error pcm_abort_transaction: " , e)        



try:
    print("Testing Open Transactions")    
    
    if ( PCMPy.pcm_open_transaction(PCMPy.PCM_TRANS_OPEN_READWRITE) >= 0 ) :        
        print("OK")
    else :
        print("Error")
except PCMPy.error as e:
    print("Error pcm_open_transaction: " , e)        


try:
    print("Testing Update register")    
    
    retorno = PCMPy.pcm_op(PCMPy.PCM_OP_WRITE_FLDS, update_register, 0)
    if ( retorno ) :
        print("OK")
        print(retorno)
        print("OK")
    else :
        print("Error")
except PCMPy.error as e:
    print("Error pcm_op: " , e)        


try:
    print("Testing Commit Transactions")    
    
    if ( PCMPy.pcm_commit_transaction() >= 0 ) :        
        print("OK")
    else :
        print("Error")
except PCMPy.error as e:
    print("Error pcm_commit_transaction: " , e)        


try:
    print("Testing pcm_pin_conf for STR value")    
    
    if ( PCMPy.pcm_pin_conf("PCMPy", "logfile", 'STR')  ) :        
        print("OK")
    else :
        print("Error")
except PCMPy.error as e:
    print("Error pcm_pin_conf: " , e)        


try:
    print("Testing pcm_pin_conf for INT value")    
    
    if ( PCMPy.pcm_pin_conf("PCMPy", "loglevel", 'INT')  ) :        
        print("OK")
    else :
        print("Error")
except PCMPy.error as e:
    print("Error pcm_pin_conf: " , e)        


try:
    print("Testing pcm_pin_conf for DECIMAL value")    
    retorno = PCMPy.pcm_pin_conf("PHP-PCM-Client", "minvalue", 'DECIMAL')
    print(retorno)
    if ( retorno ) :        
        print("OK")
    else :
        print("Error")
except PCMPy.error as e:
    print("Error pcm_pin_conf: " , e)            

try:
    print("Testing pcm_pin_conf for POID value")    
    retorno = PCMPy.pcm_pin_conf("-", "userid", 'POID')    
    if ( retorno  ) :
        print("OK")
    else :
        print("Error")        
except PCMPy.error as e:
    print("Error pcm_pin_conf: " , e)                



print(PCMPy.C_FLD_CHARGE_PENALTY_EXEMPT_FLAG)    
print(PCMPy.C_OP_COLLECTIONS_POL_EXIT_CREDIT_INSTALLMENT)    
