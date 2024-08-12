import os
from PyPDF2 import PdfReader, PdfWriter
from reportlab.pdfgen import canvas
from reportlab.lib.pagesizes import letter
import io
import json
from datetime import datetime

CommentaireCarteFonctionnelle = """• Vérification de la connectique et straps.
• Mesure et vérification des tensions appliqués sur la carte.
• Vérification de l’émission / réception.
• Vérification des cinq états de chaque voie sur les 96 voies
  (Ligne ouverte ; Congruence ; Normal ; Alarme ; Ligne en court-circuit).
• Test en endurance sur banc de test ARAL.
"""
CommentaireCarteMinimal = """• Vérification de la connectique et straps.
• Mesure et vérification des tensions appliqués sur la carte.
• Vérification de l’émission / réception.
"""

def get_drive_letter():
    drive_letter = os.getenv('SYSTEMDRIVE')
    if drive_letter:
        return str(drive_letter)
    else:
        return 'C:'
DRIVELETTER = get_drive_letter()
print(DRIVELETTER)

def get_account_username_from_path():
    # Récupérer le chemin du répertoire de l'utilisateur
    user_profile_path = os.environ['USERPROFILE']
    # Extraire le nom de l'utilisateur du chemin
    username = os.path.basename(user_profile_path)
    return str(username)
USERNAME = get_account_username_from_path()
print(USERNAME)

def ensure_directory_exists(directory_path):
    if not os.path.exists(directory_path):
        os.makedirs(directory_path)
        print(f"Directory '{directory_path}' created.")
    else:
        print(f"Directory '{directory_path}' already exists.")


output_directory = DRIVELETTER + '/Users/' + USERNAME + '/AppData/Local/BancDeTestAral'
ensure_directory_exists(output_directory)

output_pdf_directory = DRIVELETTER + '/Users/' + USERNAME + '/AppData/Local/BancDeTestAral/outputPDF'
ensure_directory_exists(output_pdf_directory)

def get_current_date_string():
    # Obtenir la date actuelle
    current_date = datetime.now()
    # Formater la date au format "dd/mm/yyyy"
    date_string = current_date.strftime("%d/%m/%Y")
    return date_string

# Fonction pour générer un numéro de série
def generer_numero_serie(prefixe, filePath = output_pdf_directory + '/compteurNumSerie.json'):
    try:
        with open(filePath, "r") as file:
            items = json.load(file)
    except (FileNotFoundError, json.JSONDecodeError):
        items = [1]

    # Obtenir la date actuelle
    maintenant = datetime.now()
    mois = maintenant.strftime("%m")
    annee = maintenant.strftime("%Y")
    semaine = maintenant.isocalendar()[1]  # Obtenir le numéro de la semaine dans l'année

    # Formater le numéro avec 3 chiffres significatifs
    compteur = f"{items[0]:03d}"
    
    # Créer le numéro de série
    numero_serie = f"{prefixe}{semaine:02d}{compteur} {mois}-{annee}"

    items[0] = (items[0] + 1) % 9999

    with open(filePath, "w") as file:
        json.dump(items, file)
    
    return numero_serie

def createPDFText(text, x, y, interligne = 30):
        # writer = PdfWriter()
        packet = io.BytesIO()
        can = canvas.Canvas(packet, pagesize=letter)
        for line in text.split('\n'):
            can.drawString(x, y, line)
            y -= interligne
        can.save()
        
        # Bouger à l'endroit du début du packet
        packet.seek(0)
        # Lire le PDF en mémoire
        new_pdf = PdfReader(packet)
        
        return new_pdf

#JSON :
def add_items_to_json(filePath, new_item):
    # Charger les éléments existants du fichier JSON
    try:
        with open(filePath, "r") as file:
            items = json.load(file)
    except (FileNotFoundError, json.JSONDecodeError):
        items = []

    # Ajouter le nouvel élément s'il n'est pas déjà dans la liste
    if new_item not in items:
        items.append(new_item)
    
    # Sauvegarder la liste mise à jour dans le fichier JSON
    with open(filePath, "w") as file:
        json.dump(items, file)
    
class FicheValidation():
    def generateFicheValidation(self, numSerie = "", controleurTechnique = "", controleurExterne = "", Commentaire = CommentaireCarteFonctionnelle, Date = None, Designation = "Carte ARAL", Reference = "Z8884306", Gamme = "40150069184002D", PosteDeTest = "Banc de test carte ARAL"):
        new_pdf = []
        
        yDeBase = 115

        # pdf = createPDFText("|______________________________", 100, 100)
        # new_pdf.append(pdf)
        # pdf = createPDFText("|______________________________", 100, 300)
        # new_pdf.append(pdf)
        pdf = createPDFText(Designation, yDeBase, 685)
        new_pdf.append(pdf)
        
        pdf = createPDFText(Reference, yDeBase, 663)
        new_pdf.append(pdf)
        
        pdf = createPDFText(numSerie, yDeBase, 642)
        new_pdf.append(pdf)
        
        pdf = createPDFText(Gamme, yDeBase, 572)
        new_pdf.append(pdf)

        pdf = createPDFText(PosteDeTest, yDeBase, 550)
        new_pdf.append(pdf)
        
        pdf = createPDFText(Commentaire, yDeBase, 529)
        new_pdf.append(pdf)
            
        pdf = createPDFText(controleurTechnique, yDeBase + 100, 142)
        new_pdf.append(pdf)
        
        pdf = createPDFText(controleurExterne, yDeBase + 100, 98)
        new_pdf.append(pdf)
        
        if(Date is None):
            Date = get_current_date_string()
        
        pdf = createPDFText(Date, yDeBase + 381, 141)
        new_pdf.append(pdf)
            
        pdf = createPDFText(Date, yDeBase + 381, 98)
        new_pdf.append(pdf)
        
        with open('PDF/fiche_validation_aral_vierge.pdf', "rb") as input_file:
            reader = PdfReader(input_file)
            writer = PdfWriter()
            
            # Ajouter le texte à chaque page du PDF existant
            for page_num in range(len(reader.pages)):
                page = reader.pages[page_num]
                for pdf in new_pdf:
                    page.merge_page(pdf.pages[0])
                writer.add_page(page)

                # Écrire le nouveau PDF sur disque
            # with open('outputPDF/output.pdf', "wb") as output_file:
            #     writer.write(output_file)
            return writer
    
    def writePDF(self, writer: PdfWriter, openPdf = True, output_path = output_pdf_directory+ '/output.pdf'):
            # Écrire le nouveau PDF sur disque
        with open(output_path, "wb") as output_file:
            writer.write(output_file)
        if openPdf:
            # Ouvrir automatiquement le PDF
            self.open_pdf(output_path)
    
    def open_pdf(self, path):
        if os.name == 'posix':
            os.system(f'open "{path}"')
        elif os.name == 'nt':
            os.startfile(path)
        elif os.name == 'mac':
            os.system(f'open "{path}"')
        else:
            print("Unsupported OS")


def test():
    
    # Reference = "Z8884306"
    # Gamme = "40150069184002D"
    # Commentaire = CommentaireCarteFonctionnelle
    
    numSerie = "1720001 06-2024"
    controleurTechnique = "Anas DAGGAG"
    controleurExterne = "Gregoire PISSOT"
    # Date = "26/06/2024"
    
    fiche = FicheValidation()
    output = fiche.generateFicheValidation(numSerie, controleurTechnique, controleurExterne)
    fiche.writePDF(output)    
    
# test()