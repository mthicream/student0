/*
 * Word count application with one thread per input file.
 *
 * You may modify this file in any way you like, and are expected to modify it.
 * Your solution must read each input file from a separate thread. We encourage
 * you to make as few changes as necessary.
 */

/*
 * Copyright © 2021 University of California, Berkeley
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <pthread.h>

#include "word_count.h"
#include "word_helpers.h"

typedef struct word_count_args {
  word_count_list_t* wclist;
  FILE* infile;
} word_count_args_t;

void* word_count_thread(void* arg) {
  word_count_args_t* args = arg;
  
  count_words(args->wclist, args->infile);

  return NULL;
}

/*
 * main - handle command line, spawning one thread per file.
 */
int main(int argc, char* argv[]) {
  int rc = 0;
  int nthreads = argc - 1;

  FILE* infile[nthreads];
  pthread_t threads[nthreads];
  word_count_args_t wcargs[nthreads];

  /* Create the empty data structure. */
  word_count_list_t word_counts;
  init_words(&word_counts);

  if (argc <= 1) {
    /* Process stdin in a single thread. */
    count_words(&word_counts, stdin);
  } else {
    /* TODO */
    for (int i = 0; i < argc - 1; i++) {
      infile[i] = fopen(argv[i + 1], "r");

      if (infile[i] == NULL) {
        fprintf(stderr, "Error opening file: %s\n", argv[i + 1]);
        continue;
      }

      wcargs[i].wclist = &word_counts;
      wcargs[i].infile = infile[i];

      
      rc = pthread_create(&threads[i], NULL, word_count_thread, (void *)&wcargs[i]);
      if (rc) {
        fprintf(stderr, "ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
      }
    }
  }

  for (int i = 0; i < argc - 1; i++) {
    pthread_join(threads[i], NULL);
    fclose(infile[i]);
  }

  /* Output final result of all threads' work. */
  wordcount_sort(&word_counts, less_count);
  fprint_words(&word_counts, stdout);
  return 0;
}
