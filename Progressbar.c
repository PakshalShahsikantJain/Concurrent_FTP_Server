#include <stdio.h>
#include <unistd.h> // For usleep()

void print_progress(int current, int total) {
    int width = 50; // Total width of the bar
    float progress = (float)current / total;
    int filled = (int)(progress * width);
    int i = 0;

    // \r moves cursor to start; [ is the bar start
    printf("\rProgress: ["); 
    
    for (int i = 0; i < filled; i++) 
    {
        printf("#");
    }
    
    for (int i = filled; i < width; i++)
    {
        printf(" ");
    }
    
    printf("] %d%%", (int)(progress * 100));
    
    // Force the terminal to show the output immediately
    fflush(stdout); 
}

int main() {
    int total = 1024;
    for (int i = 0; i <= total; i++) {
        print_progress(i, total);
        usleep(100000); // 50ms delay to simulate work
    }
    printf("\nDone!\n"); // Final newline when finished
    return 0;
}
