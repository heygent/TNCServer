#ifndef TNC_ERROR_H
#define TNC_ERROR_H

enum TNCLib_error {

  TNCError_good = 0,                  /** La funzione è terminata con esito
                                        positivo. */

  TNCError_failed_alloc = 1,          /** Un'allocazione di memoria necessaria
                                        per lo svolgimento della funzione è
                                        fallita. */
    
  TNCError_thread_start_failed,       /** L'avvio di un thread nella funzione è
                                        fallito. */
  
  TNCError_module_defined             /** Indica l'inizio dei valori di errori
                                        dipendenti dal modulo. */

};

#endif //TNC_ERROR_H
