#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FPS 60

#define WINDOWHEIGHT 900
#define WINDOWWIDTH 1200

#define COLUMNWIDTH WINDOWWIDTH / 3

#define x_index 680
#define y_index 820


const int colors[3][4] = {
    {155, 184, 205, 255},
    {238, 199, 89, 255},
    {177, 195, 129, 255},
};
void renderColumnTitles(SDL_Renderer *renderer, TTF_Font *font, int columnWidth, int windowHeight)
{
    char *titles[] = {"TO DO", "IN WORK", "DONE"};
    SDL_Color textColor = {255, 255, 255, 255}; // White text

    for (int i = 0; i < 3; ++i)
    {
        SDL_Rect backGround = {columnWidth * i, 0, columnWidth, 50};
        SDL_SetRenderDrawColor(renderer, colors[i][0], colors[i][1], colors[i][2], colors[i][3]);
        SDL_RenderFillRect(renderer, &backGround);

        SDL_Surface *surface = TTF_RenderText_Solid(font, titles[i], textColor);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

        int textWidth = surface->w;
        int textHeight = surface->h;
        SDL_FreeSurface(surface);

        SDL_Rect textRect = {columnWidth * i + (columnWidth - textWidth) / 2, 10, textWidth, textHeight};
        SDL_RenderCopy(renderer, texture, NULL, &textRect);

        SDL_DestroyTexture(texture);
    }
}
void drawColumns(SDL_Renderer *renderer, int columnWidth, int windowHeight, TTF_Font *font)
{
    SDL_Rect column1 = {0, 0, columnWidth, windowHeight};
    SDL_Rect column2 = {columnWidth, 0, columnWidth, windowHeight};
    SDL_Rect column3 = {columnWidth * 2, 0, columnWidth, windowHeight};

    // Set color and draw column 1
    SDL_SetRenderDrawColor(renderer, colors[0][0], colors[0][1], colors[0][2], colors[0][3]); // Red
    SDL_RenderFillRect(renderer, &column1);

    // Set color and draw column 2
    SDL_SetRenderDrawColor(renderer, colors[1][0], colors[1][1], colors[1][2], colors[1][3]); // Green
    SDL_RenderFillRect(renderer, &column2);

    // Set color and draw column 3
    SDL_SetRenderDrawColor(renderer, colors[2][0], colors[2][1], colors[2][2], colors[2][3]); // Blue
    SDL_RenderFillRect(renderer, &column3);
}

SDL_Texture *List_Textures[1024];
SDL_Surface *List_Surfaces[1024];
int compteur = 0;

typedef struct todoElements
{
    SDL_Rect element;
    SDL_Rect text;

    char textContent[1024];
    char fontPath[1024]; // Path to the font file
    int fontSize;
    SDL_Color color;

    int index;

    int isDragging;
    struct todoElements *next;
} todoElements;

void AddElement(todoElements **list, SDL_Renderer *renderer, SDL_Rect newele, char text[1024])
{
    todoElements *item = malloc(sizeof(todoElements));
    item->element = newele;
    strcpy(item->textContent, text);
    strcpy(item->fontPath, "./Roboto-Regular.ttf");
    item->fontSize = 24;
    SDL_Color color = {255, 255, 255, 255};
    item->color = color;
    item->index = compteur;
    List_Surfaces[compteur] = TTF_RenderText_Solid(TTF_OpenFont(item->fontPath, item->fontSize), item->textContent, item->color);
    List_Textures[compteur] = SDL_CreateTextureFromSurface(renderer, List_Surfaces[compteur]);
    compteur++;
    item->next = *list;
    *list = item;
}

void drawElements(todoElements *list, SDL_Renderer *renderer)
{
    if (list == NULL)
    {
        return;
    }
    if (list->next == NULL)
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black
        SDL_RenderFillRect(renderer, &list->element);

        SDL_Rect renderQuad = {list->element.x + 10, list->element.y + 15, List_Surfaces[list->index]->w, List_Surfaces[list->index]->h};

        SDL_RenderCopy(renderer, List_Textures[list->index], NULL, &renderQuad);

        return;
    }
    drawElements(list->next, renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black
    SDL_RenderFillRect(renderer, &list->element);

    SDL_Rect renderQuad = {list->element.x + 10, list->element.y + 15, List_Surfaces[list->index]->w, List_Surfaces[list->index]->h};

    SDL_RenderCopy(renderer, List_Textures[list->index], NULL, &renderQuad);
}

typedef struct AddButton
{
    int x;
    int y;
} AddButton;

void drawButton(SDL_Renderer *renderer, AddButton *button, TTF_Font *font)
{
    SDL_Rect boutton = {button->x, button->y - 60, 140, 40};
    SDL_SetRenderDrawColor(renderer, 1, 1, 1, 255);
    SDL_RenderFillRect(renderer, &boutton);

    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Solid(font, "New task", textColor);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    int text_width = surface->w;
    int text_height = surface->h;
    SDL_FreeSurface(surface);

    SDL_Rect renderQuad = {button->x + 20, button->y - text_height - 28, text_width, text_height};
    SDL_RenderCopy(renderer, texture, NULL, &renderQuad);
}

typedef struct inputText
{
    char text[1024];
    int index;
} inputText;

void contentMain(SDL_Renderer *renderer, TTF_Font *font, todoElements *list, AddButton button, SDL_Rect *zone, inputText *input)
{

    SDL_RenderClear(renderer);
    // Draw columns
    drawColumns(renderer, COLUMNWIDTH, WINDOWHEIGHT, font);

    SDL_Rect backNewTask = {x_index - 10, y_index - 10, WINDOWWIDTH / 3 - 60 + 20, 80};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &backNewTask);

    SDL_Color textColorNewTask = {128, 128, 128, 255};
    SDL_Surface *surfaceNewTask = TTF_RenderText_Solid(font, "New Task Here", textColorNewTask);
    SDL_Texture *textureNewTask = SDL_CreateTextureFromSurface(renderer, surfaceNewTask);

    int text_widthNewTask = surfaceNewTask->w;
    int text_heightNewTask = surfaceNewTask->h;
    SDL_FreeSurface(surfaceNewTask);

    SDL_Rect renderQuadNewTask = {backNewTask.x + (backNewTask.w / 2)/2 + 15, backNewTask.y + (backNewTask.h / 2)/2 + 5, text_widthNewTask, text_heightNewTask};

    SDL_RenderCopy(renderer, textureNewTask, NULL, &renderQuadNewTask);

    // Draw content
    drawElements(list, renderer);


    if (zone != NULL)
    {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, zone);
    }
    if (input != NULL)
    {
        if (input->index > 0)
        {
            SDL_Color textColor = {0, 0, 0, 255}; // Black text
            SDL_Surface *surface = TTF_RenderText_Solid(font, input->text, textColor);
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

            int text_width = surface->w;
            int text_height = surface->h;
            SDL_FreeSurface(surface);

            SDL_Rect renderQuad = {zone->x + 10, zone->y + ((60 - text_height) / 2), text_width, text_height};
            SDL_RenderCopy(renderer, texture, NULL, &renderQuad);
        }
    }


    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    drawButton(renderer, &button, font);

    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Solid(font, "Right Clic to Del", textColor);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    int text_width = surface->w;
    int text_height = surface->h;
    SDL_FreeSurface(surface);

    SDL_Rect renderQuad = {button.x - 10, button.y - text_height - 28 + 35, text_width, text_height};
    SDL_RenderCopy(renderer, texture, NULL, &renderQuad);

    renderColumnTitles(renderer, font, COLUMNWIDTH, WINDOWHEIGHT);
    // Update screen
    SDL_RenderPresent(renderer);
    SDL_Delay(1000 / FPS);
}


void saveTodoElementsToFile(todoElements *head, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        perror("Error opening file for writing");
        return;
    }

    for (todoElements *current = head; current != NULL; current = current->next)
    {
        fprintf(file, "%d,%d,%d,%d,%s\n",
                current->element.x, current->element.y, current->element.w, current->element.h,
                current->textContent);
    }

    fclose(file);
}

void loadTodoElementsFromFile(const char *filename, todoElements *list, SDL_Renderer *renderer)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Error opening file for reading");
        return;
    }

    int var1, var2, var3, var4;
    char string1[1024];

    while (1)
    {
        if (fscanf(file, "%d,%d,%d,%d,%255s\n",
                   &var1, &var2, &var3, &var4, string1) != 5)
        {
            break;
        }

        SDL_Rect element = {var1, var2, var3, var4};
        AddElement(list, renderer, element, string1);
    }
    fclose(file);
}


void deleteElement(todoElements** head, int valueToDelete) {
    // If the list is empty, there is nothing to delete
    if (*head == NULL) {
        return;
    }

    todoElements* temp = *head;
    todoElements* prev = NULL;

    // If the head node itself holds the value to be deleted
    if (temp != NULL && temp->index == valueToDelete) {
        *head = temp->next;   // Change head
        free(temp);           // Free old head
        return;
    }

    // Search for the value to be deleted, keep track of the previous node
    while (temp != NULL && temp->index != valueToDelete) {
        prev = temp;
        temp = temp->next;
    }

    // If value was not present in linked list
    if (temp == NULL) {
        return;
    }

    // Unlink the node from linked list
    prev->next = temp->next;

    free(temp);  // Free memory
}

int main(int argc, char *argv[])
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    if (TTF_Init() == -1)
    {
        SDL_Log("TTF_Init: %s\n", TTF_GetError());
        // handle error
    }

    TTF_Font *font = TTF_OpenFont("./Roboto-Regular.ttf", 24); // Modify the path and size
    if (!font)
    {
        SDL_Log("TTF_OpenFont: %s\n", TTF_GetError());
        // handle error
    }

    // Create a window
    SDL_Window *window = SDL_CreateWindow("SDL Todo List",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          WINDOWWIDTH, WINDOWHEIGHT, SDL_WINDOW_SHOWN);
    if (!window)
    {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    todoElements *list = NULL;
    loadTodoElementsFromFile("./todo.csv", &list, renderer);

    SDL_Rect rect = {(int)0, (int)0, WINDOWWIDTH / 3, WINDOWHEIGHT};

    SDL_Event event;
    int running = 1;

    int mouseX = 0;
    int mouseY = 0;

    AddButton button = {WINDOWWIDTH - 160, WINDOWHEIGHT - 10};

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                running = 0;
                break;

            case SDL_MOUSEBUTTONDOWN:
                SDL_GetMouseState(&mouseX, &mouseY);
                if (mouseX >= button.x && mouseX <= button.x + 140 && mouseY >= button.y - 60 && mouseY <= button.y - 20)
                {
                    SDL_Color textColor = {255, 255, 255, 255}; // White text

                    inputText input = 
                    {
                        .text = "",
                        .index = 0,
                    };

                    int runInput = 0;
                    while (runInput == 0)
                    {
                        SDL_Event event;
                        SDL_WaitEvent(&event);

                        SDL_Rect zone = {x_index, y_index, WINDOWWIDTH / 3 - 60, 60};

                        contentMain(renderer, font, list, button, &zone, &input);

                        if (event.type == SDL_KEYDOWN)
                        {
                            if (event.key.keysym.scancode == SDL_SCANCODE_RETURN)
                            {
                                if (input.index == 0)
                                {
                                    strcat(input.text, "_");
                                }
                                break;

                                runInput = 1;
                            }
                            else if (event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE)
                            {
                                if (input.index > 0)
                                {
                                    input.text[input.index - 1] = '\0';
                                    input.index--;
                                }
                            }
                            else if (event.key.keysym.scancode == SDL_SCANCODE_SPACE)
                            {
                                if (input.index < 25)
                                {
                                    input.text[input.index] = '_';
                                    input.index++;
                                }
                            }
                            else
                            {
                                if (input.index < 25)
                                {
                                    input.text[input.index] = event.key.keysym.sym;
                                    input.index++;
                                }
                            }
                        }
                    }
                    SDL_Rect new = {x_index, y_index, WINDOWWIDTH / 3 - 60, 60};

                    SDL_Surface *surface = TTF_RenderText_Solid(font, input.text, textColor);
                    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

                    int text_width = surface->w;
                    int text_height = surface->h;
                    SDL_FreeSurface(surface);

                    SDL_Rect renderQuad = {new.x + 10, y_index + 15, text_width, text_height};
                    AddElement(&list, renderer, new, input.text);

                }
                if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    for (todoElements *current = list; current != NULL; current = current->next)
                    {
                        if (SDL_PointInRect(&(SDL_Point){event.button.x, event.button.y}, &current->element))
                        {
                            deleteElement(&list, current->index);
                        }
                    }
                }
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    for (todoElements *current = list; current != NULL; current = current->next)
                    {
                        if (SDL_PointInRect(&(SDL_Point){event.button.x, event.button.y}, &current->element))
                        {
                            current->isDragging = 1;
                        }
                    }
                }
                break;
            case SDL_MOUSEBUTTONUP:
                for (todoElements *current = list; current != NULL; current = current->next)
                {
                    current->isDragging = 0;
                }
                break;
            case SDL_MOUSEMOTION:
                for (todoElements *current = list; current != NULL; current = current->next)
                {
                    if (current->isDragging)
                    {
                        current->element.x += event.motion.xrel;
                        current->element.y += event.motion.yrel;
                    }
                }
                break;
               
            }
        }

        contentMain(renderer, font, list, button, NULL, NULL);
    }

    // Cleanup
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    saveTodoElementsToFile(list, "./todo.csv");
    return 0;
}