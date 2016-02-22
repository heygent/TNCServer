#ifndef TNC_ERROR_H
#define TNC_ERROR_H

/** @file
 * @brief Contiene i codici di errore restituiti dalle funzioni. 
 */

enum TNCError {
  
  /** La funzione è terminata con esito positivo. */
  TNCError_good = 0,

  /** Un'allocazione di memoria necessaria per l'esecuzione della funzione è
   * fallita. */    
  TNCError_failed_alloc = 1, 

  /** L'avvio di un thread nella funzione è fallito. */ 
  TNCError_thread_start_failed,
  
  /** Indica l'inizio dei valori di errori dipendenti dal modulo. */ 
  TNCError_module_defined 

};

#endif //TNC_ERROR_H
