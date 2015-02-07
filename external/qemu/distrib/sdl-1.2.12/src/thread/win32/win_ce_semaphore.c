

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "win_ce_semaphore.h"

static SYNCHHANDLE CleanUp (SYNCHHANDLE hSynch, DWORD Flags);

SYNCHHANDLE CreateSemaphoreCE (

   LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,  /* pointer to security attributes */
      LONG lInitialCount,   /* initial count */
      LONG lMaximumCount,   /* maximum count */
      LPCTSTR lpName )


{
   SYNCHHANDLE hSynch = NULL, result = NULL;

   __try
	{
      if (lInitialCount > lMaximumCount || lMaximumCount < 0 || lInitialCount < 0) 
	  {
              /* Bad parameters */
         SetLastError (SYNCH_ERROR);
         __leave;
      }

      hSynch = HeapAlloc (GetProcessHeap(), HEAP_ZERO_MEMORY, SYNCH_HANDLE_SIZE);
      if (hSynch == NULL) __leave;

      hSynch->MaxCount = lMaximumCount;
      hSynch->CurCount = lInitialCount;
      hSynch->lpName = lpName;

      hSynch->hMutex = CreateMutex (lpSemaphoreAttributes, FALSE, NULL);

      WaitForSingleObject (hSynch->hMutex, INFINITE);
      /*  Create the event. It is initially signaled if and only if the
          initial count is > 0 */
      hSynch->hEvent = CreateEvent (lpSemaphoreAttributes, FALSE, 
              lInitialCount > 0, NULL);
      ReleaseMutex (hSynch->hMutex);
      hSynch->hSemph = NULL;
   }
   __finally
   {
       /* Return with the handle, or, if there was any error, return
        a null after closing any open handles and freeing any allocated memory. */
      result=CleanUp(hSynch, 6 /* An event and a mutex, but no semaphore. */);
   }

   return result;
}

BOOL ReleaseSemaphoreCE (SYNCHHANDLE hSemCE, LONG cReleaseCount, LPLONG lpPreviousCount)
/* Windows CE equivalent to ReleaseSemaphore. */
{
   BOOL Result = TRUE;

   /* Gain access to the object to assure that the release count
      would not cause the total count to exceed the maximum. */

   __try 
   {
      WaitForSingleObject (hSemCE->hMutex, INFINITE);
	  /* reply only if asked to */	
	  if (lpPreviousCount!=NULL)
		 *lpPreviousCount = hSemCE->CurCount;
      if (hSemCE->CurCount + cReleaseCount > hSemCE->MaxCount || cReleaseCount <= 0)
	  {
         SetLastError (SYNCH_ERROR);
         Result = FALSE;
         __leave;
      }
      hSemCE->CurCount += cReleaseCount;

      /*  Set the autoreset event, releasing exactly one waiting thread, now or
          in the future.  */

      SetEvent (hSemCE->hEvent);
   }
   __finally
   {
      ReleaseMutex (hSemCE->hMutex);
   }

   return Result;
}

DWORD WaitForSemaphoreCE (SYNCHHANDLE hSemCE, DWORD dwMilliseconds)
   /* Windows CE semaphore equivalent of WaitForSingleObject. */
{
   DWORD WaitResult;

   WaitResult = WaitForSingleObject (hSemCE->hMutex, dwMilliseconds);
   if (WaitResult != WAIT_OBJECT_0 && WaitResult != WAIT_ABANDONED_0) return WaitResult;
   while (hSemCE->CurCount <= 0) 
   { 

      /* The count is 0, and the thread must wait on the event (which, by
         the rules, is currently reset) for semaphore resources to become
         available. First, of course, the mutex must be released so that another
         thread will be capable of setting the event. */

      ReleaseMutex (hSemCE->hMutex);

      /*  Wait for the event to be signaled, indicating a semaphore state change.
          The event is autoreset and signaled with a SetEvent (not PulseEvent)
          so exactly one waiting thread (whether or not there is currently
          a waiting thread) is released as a result of the SetEvent. */

      WaitResult = WaitForSingleObject (hSemCE->hEvent, dwMilliseconds);
      if (WaitResult != WAIT_OBJECT_0) return WaitResult;

      /*  This is where the properties of setting of an autoreset event is critical
          to assure that, even if the semaphore state changes between the
          preceding Wait and the next, and even if NO threads are waiting
          on the event at the time of the SetEvent, at least one thread
          will be released. 
          Pulsing a manual reset event would appear to work, but it would have
          a defect which could appear if the semaphore state changed between
          the two waits. */

      WaitResult = WaitForSingleObject (hSemCE->hMutex, dwMilliseconds);
      if (WaitResult != WAIT_OBJECT_0 && WaitResult != WAIT_ABANDONED_0) return WaitResult;

   }
   /* The count is not zero and this thread owns the mutex.  */

   hSemCE->CurCount--;
   /* The event is now unsignaled, BUT, the semaphore count may not be
      zero, in which case the event should be signaled again
      before releasing the mutex. */

   if (hSemCE->CurCount > 0) SetEvent (hSemCE->hEvent);
   ReleaseMutex (hSemCE->hMutex);
   return WaitResult;
}

BOOL CloseSynchHandle (SYNCHHANDLE hSynch)
{
   BOOL Result = TRUE;
   if (hSynch->hEvent != NULL) Result = Result && CloseHandle (hSynch->hEvent);
   if (hSynch->hMutex != NULL) Result = Result && CloseHandle (hSynch->hMutex);
   if (hSynch->hSemph != NULL) Result = Result && CloseHandle (hSynch->hSemph);
   HeapFree (GetProcessHeap (), 0, hSynch);
   return (Result);
}

static SYNCHHANDLE CleanUp (SYNCHHANDLE hSynch, DWORD Flags)
{ /* Prepare to return from a create of a synchronization handle.
     If there was any failure, free any allocated resources.
     "Flags" indicates which Win32 objects are required in the 
     synchronization handle. */

   BOOL ok = TRUE;

   if (hSynch == NULL) return NULL;
   if ((Flags & 4) == 1 && (hSynch->hEvent == NULL)) ok = FALSE;
   if ((Flags & 2) == 1 && (hSynch->hMutex == NULL)) ok = FALSE;
   if ((Flags & 1) == 1 && (hSynch->hEvent == NULL)) ok = FALSE;
   if (!ok) 
   {
      CloseSynchHandle (hSynch);
      return NULL;
   }
   /* Everything worked */
   return hSynch;
}
