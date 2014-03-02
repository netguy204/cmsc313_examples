typedef struct book {
  char title[48] ;
  char author[48] ;            // first author
  char subject[20] ;           // Nonfiction, Fantasy, Mystery, ...
  unsigned int year ;          // year of e-book release
  struct book *next ;
} book ;

book l1 = {"Breaking Point", "Pamela Clare", "Romance", 2011, (void*)0};
book l2 = {"Vow", "Kim Carpenter", "Nonfiction", 2012, &l1};
book l3 = {"1491", "Charles C. Mann", "Nonfiction", 2006, &l2};
book l4 = {"Three Weeks with My Brother", "Nicholas Sparks", "Nonfiction", 2004, &l3};
book l5 = {"The 10 Habits of Happy Mothers", "Meg Meeker, M.D.", "Nonfiction", 2011, &l4};
book l6 = {"We Need to Talk About Kevin", "Lionel Shriver", "Fiction", 2011, &l5};
book l7 = {"The Prague Cemetery", "Umberto Eco", "Fiction", 2011, &l6};
book l8 = {"Black Dahlia & White Rose", "Joyce Carol Oates", "Fiction", 2012, &l7};
book l9 = {"Glad Tidings", "Debbie Macomber", "Romance", 2012, &l8};
book l10 = {"A Summer Affair", "Elin Hilderbrand", "Romance", 2008, &l9};
book l11 = {"Dead until Dark", "Charlaine Harris", "Fantasy", 2001, &l10};
book l12 = {"Between the Dark and the Daylight", "Richard Marsh", "Mystery", 2012, &l11};

long breaking_up_the_memory = 42;

book l13 = {"Good Girls Don't", "Victoria Dahl", "Romance", 2011, &l12};
book l14 = {"Inheritance", "Christopher Paolini", "Fantasy", 2011, &l13};
book l15 = {"Best Staged Plans", "Claire Cook", "Fiction", 2011, &l14};
book l16 = {"The Gap Year", "Sarah Bird", "Humor", 2011, &l15};
book l17 = {"The Affair Next Door", "Anna Katherine Green", "Mystery", 2012, &l16};
book l18 = {"Crossed", "Ally Condie", "Romance", 2011, &l17};
book l19 = {"Eclipse", "Stephenie Meyer", "Romance", 2007, &l18};
book l20 = {"New Moon", "Stephenie Meyer", "Romance", 2007, &l19};
book l21 = {"The Third Gate", "Lincoln Child", "Fantasy", 2012, &l20};
book l22 = {"1225 Christmas Tree Lane", "Debbie Macomber", "Romance", 2011, &l21};
book l23 = {"Murder on Astor Place", "Victoria Thompson", "Mystery", 2009, &l22};
book l24 = {"Heaven is for Real", "Todd Burpo", "Nonfiction", 2011, &l23};
book l25 = {"15 Seconds", "Andrew Gross", "Thriller", 2012, &l24};
book l26 = {"Harry Potter and the Chamber of Secrets", "J.K. Rowling", "Fantasy", 2012, &l25};
book l27 = {"1Q84", "Haruki Murakami", "Fiction", 2011, &l26};
book l28 = {"The Affair", "Lee Child", "Mystery", 2011, &l27};
book l29 = {"The Girl With the Dragon Tattoo", "Stieg Larsson", "Mystery", 2011, &l28};
book l30 = {"Explosive Eighteen", "Janet Evanovich", "Mystery", 2011, &l29};
book l31 = {"The Help", "Kathryn Stockett", "Fiction", 2009, &l30};

book* library = &l31;
