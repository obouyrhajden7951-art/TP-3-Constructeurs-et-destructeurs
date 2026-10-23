/*
 * Main.cpp
 *
 *  Created on: 23 oct. 2026
 *      Author: hP
 */

#include <iostream>
#include <string>
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <utility>
#include <cstddef>

// ===================== 1. COMPTE BANCAIRE =====================
class CompteBancaire {
private:
    std::string numero; // @suppress("Type cannot be resolved")
    double solde;
    std::string* historique; // @suppress("Type cannot be resolved")
    std::size_t capacite;
    std::size_t nbOps;

    void agrandirHistorique() {
        std::size_t nouvelleCap = (capacite == 0) ? 10 : capacite * 2;
        std::string* nouveau = new std::string[nouvelleCap];
        std::copy(historique, historique + nbOps, nouveau);
        delete[] historique;
        historique = nouveau;
        capacite = nouvelleCap;
    }

    void ajouterOperation(const std::string& op) {
        if (nbOps == capacite) agrandirHistorique();
        historique[nbOps++] = op;
    }

public:
    // Constructeur par defaut
    CompteBancaire() : numero("NON_DEFINI"), solde(0.0), capacite(10), nbOps(0) {
        historique = new std::string[capacite];
    }

    // Constructeur parametre
    CompteBancaire(const std::string& num, double s)
        : numero(num), solde(s), capacite(10), nbOps(0) {
        historique = new std::string[capacite];
        ajouterOperation("Creation du compte avec solde " + std::to_string(s));
    }

    // Constructeur de copie
    CompteBancaire(const CompteBancaire& other)
        : numero(other.numero), solde(other.solde),
          capacite(other.capacite), nbOps(other.nbOps) {
        historique = new std::string[capacite];
        std::copy(other.historique, other.historique + nbOps, historique);
        std::cout << "Constructeur de copie : compte " << numero << std::endl;
    }

    // Constructeur de deplacement
    CompteBancaire(CompteBancaire&& other) noexcept
        : numero(std::move(other.numero)), solde(other.solde),
          historique(other.historique), capacite(other.capacite), nbOps(other.nbOps) {
        other.historique = nullptr;
        other.nbOps = 0;
        other.capacite = 0;
        other.solde = 0.0;
        std::cout << "Constructeur de deplacement : compte " << numero << std::endl;
    }

    // Assignation de copie
    CompteBancaire& operator=(const CompteBancaire& other) {
        if (this != &other) {
            std::string* nouveau = new std::string[other.capacite];
            std::copy(other.historique, other.historique + other.nbOps, nouveau);
            delete[] historique;
            historique = nouveau;
            numero = other.numero;
            solde = other.solde;
            capacite = other.capacite;
            nbOps = other.nbOps;
        }
        return *this;
    }

    // Assignation de deplacement
    CompteBancaire& operator=(CompteBancaire&& other) noexcept {
        if (this != &other) {
            delete[] historique;
            numero = std::move(other.numero);
            solde = other.solde;
            historique = other.historique;
            capacite = other.capacite;
            nbOps = other.nbOps;
            other.historique = nullptr;
            other.capacite = 0;
            other.nbOps = 0;
            other.solde = 0.0;
        }
        return *this;
    }

    // Destructeur
    ~CompteBancaire() {
        if (historique == nullptr)
            std::cout << "Destruction d'un compte vide (deplace)" << std::endl;
        else
            std::cout << "Destruction du compte " << numero << std::endl;
        delete[] historique;
    }

    void deposer(double montant) {
        if (montant <= 0) {
            std::cerr << "Montant de depot invalide" << std::endl;
            return;
        }
        solde += montant;
        ajouterOperation("Depot de " + std::to_string(montant));
    }

    void retirer(double montant) {
        if (montant <= 0 || montant > solde) {
            std::cerr << "Retrait impossible" << std::endl;
            return;
        }
        solde -= montant;
        ajouterOperation("Retrait de " + std::to_string(montant));
    }

    void afficher() const {
        std::cout << "Compte " << numero << " - Solde : " << solde << std::endl;
        std::cout << "Historique des operations :" << std::endl;
        for (std::size_t i = 0; i < nbOps; ++i) {
            std::cout << "  " << historique[i] << std::endl;
        }
    }
};

// ===================== 2. BUFFER =====================
class Buffer {
private:
    char* data;
    std::size_t taille;

public:
    Buffer() : data(nullptr), taille(0) {}

    explicit Buffer(std::size_t t) : data(new char[t]), taille(t) {
        std::fill_n(data, taille, 0);
        std::cout << "Buffer de " << taille << " octets cree" << std::endl;
    }

    Buffer(const Buffer& other) : data(new char[other.taille]), taille(other.taille) {
        std::copy(other.data, other.data + taille, data);
        std::cout << "Constructeur de copie du buffer" << std::endl;
    }

    Buffer(Buffer&& other) noexcept : data(other.data), taille(other.taille) {
        other.data = nullptr;
        other.taille = 0;
        std::cout << "Constructeur de deplacement du buffer" << std::endl;
    }

    Buffer& operator=(const Buffer& other) {
        if (this != &other) {
            delete[] data;
            taille = other.taille;
            data = new char[taille];
            std::copy(other.data, other.data + taille, data);
        }
        return *this;
    }

    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            delete[] data;
            data = other.data;
            taille = other.taille;
            other.data = nullptr;
            other.taille = 0;
        }
        return *this;
    }

    ~Buffer() {
        delete[] data;
        std::cout << "Destruction du buffer" << std::endl;
    }

    char* getData() const { return data; }
    std::size_t getTaille() const { return taille; }
};

// ===================== 3. POINT3D (membres const) =====================
class Point3D {
private:
    const double x;
    const double y;
    const double z;

public:
    Point3D(double a, double b, double c) : x(a), y(b), z(c) {}
    double getX() const { return x; }
    double getY() const { return y; }
    double getZ() const { return z; }
};

// ===================== 4. BUFFER RAII =====================
class BufferRAII {
private:
    std::unique_ptr<char[]> data;
    std::size_t taille;

public:
    explicit BufferRAII(std::size_t t) : data(new char[t]), taille(t) {
        std::fill_n(data.get(), taille, 0);
    }
    char* get() const { return data.get(); }
    std::size_t size() const { return taille; }
};

// ===================== 5. COMPTE SECURE (exceptions) =====================
class CompteSecure {
private:
    double solde;

public:
    explicit CompteSecure(double soldeInitial) {
        if (soldeInitial < 0) {
            throw std::invalid_argument("Solde initial negatif");
        }
        solde = soldeInitial;
    }
    double getSolde() const { return solde; }
};

// ===================== FONCTIONS DE TEST =====================
void testCompte() {
    std::cout << "===== Test CompteBancaire =====" << std::endl;
    CompteBancaire c1("FR123456", 500.0);
    c1.deposer(200);
    c1.retirer(150);
    c1.afficher();

    CompteBancaire c2 = c1;            // copie
    c2.afficher();

    CompteBancaire c3 = std::move(c1); // deplacement
    c3.afficher();

    CompteBancaire c4;                 // constructeur par defaut
    c4.deposer(50);
    c4.afficher();
}

void testBuffer() {
    std::cout << "\n===== Test Buffer =====" << std::endl;
    Buffer b1(128);
    Buffer b2 = b1;                    // copie
    Buffer b3 = std::move(b1);         // deplacement
    std::cout << "Taille de b1 apres deplacement : " << b1.getTaille() << std::endl;
    b2 = b3;                           // assignation de copie
    b3 = Buffer(64);                   // assignation de deplacement
    std::cout << "Taille de b3 : " << b3.getTaille() << std::endl;
}

void testPoint() {
    std::cout << "\n===== Test Point3D =====" << std::endl;
    Point3D p(1.0, 2.0, 3.0);
    std::cout << "Coordonnees : (" << p.getX() << ", " << p.getY()
              << ", " << p.getZ() << ")" << std::endl;
}

void testRAII() {
    std::cout << "\n===== Test BufferRAII =====" << std::endl;
    BufferRAII b(256);
    std::cout << "Buffer de " << b.size() << " octets alloue" << std::endl;
}

void testSecure() {
    std::cout << "\n===== Test CompteSecure =====" << std::endl;
    try {
        CompteSecure ok(100.0);
        std::cout << "Compte cree, solde = " << ok.getSolde() << std::endl;
        CompteSecure ko(-5.0);         // lance une exception
        std::cout << "Cette ligne ne s'affiche pas" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Erreur : " << e.what() << std::endl;
    }
}

int main() {
    testCompte();
    testBuffer();
    testPoint();
    testRAII();
    testSecure();
    return 0;
}



