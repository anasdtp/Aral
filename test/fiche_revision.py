import io
from PyPDF2 import PdfReader, PdfWriter
from reportlab.pdfgen import canvas
from reportlab.lib.pagesizes import letter

# Path to the output PDF
output_pdf_path = "C:/Users/anasd/Documents/PlatformIO/Projects/Banc_de_test_carte_ARAL/test/Fiches_de_Revision_Droit.pdf"

# Create a PDF buffer
packet = io.BytesIO()

# Create a canvas to write content
can = canvas.Canvas(packet, pagesize=letter)

# Define fiches content
fiches = [
    ("Fiche 1 : Responsabilité Civile Générale",
     [
         "- Objectif : Demander la réparation d’un préjudice subi.",
         "- Types de demandes : Remise en état, Dommages-intérêts.",
         "- Éléments requis pour la responsabilité : Dommage avéré, ",
         "Auteur du dommage, Lien de causalité.",
         "- Principe de responsabilité : Indépendamment de la bonne foi.",
         "- Responsabilité des objets et des personnes sous autorité : Notion de « garde de la chose »."
     ],
     "Pages 1-3"),

    ("Fiche 2 : Comité Social et Économique (CSE)",
     [
         "- Remplacement du CHSCT : Intègre les fonctions du CHSCT, DP, et CE.",
         "- Missions du CSE : Représentation des salariés, Promotion de la santé et sécurité, ",
         "Expression collective, Consultation obligatoire, Droit d'alerte.",
         "- Consultation obligatoire : Sur les orientations stratégiques, restructurations, licenciements collectifs.",
         "- Droit d'alerte : En cas de danger grave, atteinte aux droits des personnes."
     ],
     "Pages 8-12"),

    ("Fiche 3 : L’Inspecteur du Travail",
     [
         "- Mission : Veiller au respect du droit, pouvoir de décision administrative, et assistance ",
         "aux salariés et employeurs.",
         "- Missions principales : Respect du droit, Pouvoirs de l'inspecteur, Pouvoir de décision, ",
         "Assistance et conciliation."
     ],
     "Pages 8-10"),

    ("Fiche 4 : Règlement Intérieur",
     [
         "- Obligation : Imposé dans les entreprises de 50 salariés ou plus.",
         "- Contenu : Discipline, Santé et Sécurité, Prévention des harcèlements, Droit de défense des salariés.",
         "- Consultation : Avis du CSE nécessaire avant adoption.",
         "- Affichage : Obligatoire pour assurer l’information des salariés."
     ],
     "Pages 16-17"),

    ("Fiche 5 : Harcèlement Moral et Sexuel",
     [
         "- Définition : Harcèlement moral et Harcèlement sexuel.",
         "- Responsabilité de l’employeur : Obligation de prévention, de traitement des situations de harcèlement.",
         "- Sanctions : Disciplinaires (licenciement), pénales (amendes, emprisonnement)."
     ],
     "Pages 9-10"),

    ("Fiche 6 : Licenciement",
     [
         "- Types de licenciement : Pour motif personnel, Pour motif économique.",
         "- Procédure : Convocation, Entretien préalable, Notification du licenciement, Préavis.",
         "- Indemnités : Selon la cause du licenciement et l’ancienneté du salarié."
     ],
     "Pages 7-9"),

    ("Fiche 7 : Contrat de Travail",
     [
         "- Caractéristiques : Lien de subordination, Contrepartie financière, Prestation de travail.",
         "- Types de contrat : CDI, CDD, CTT.",
         "- Clauses spécifiques : Période d'essai, Clause de non-concurrence, Clause de mobilité."
     ],
     "Pages 7-9"),

    ("Fiche 8 : Congés Payés",
     [
         "- Droit aux congés payés : 2,5 jours ouvrables par mois de travail effectif.",
         "- Prise des congés : Période de référence, Ordre des départs.",
         "- Congés exceptionnels : Naissance, mariage, décès.",
         "- Indemnisation : Calculée sur la base du salaire habituel."
     ],
     "Pages 9-11"),

    ("Fiche 9 : Heures Supplémentaires",
     [
         "- Définition : Heures effectuées au-delà de la durée légale du travail.",
         "- Majoration : 25% pour les 8 premières heures, 50% pour les suivantes.",
         "- Contingent annuel : Nombre d’heures supplémentaires possible.",
         "- Repos compensateur : Droit à un repos supplémentaire."
     ],
     "Pages 9-11"),

    ("Fiche 10 : Temps de Travail et Repos",
     [
         "- Durée légale du travail : 35 heures par semaine.",
         "- Durée maximale : Journalière de 10 heures, Hebdomadaire de 48 heures.",
         "- Repos quotidien : 11 heures consécutives minimum.",
         "- Repos hebdomadaire : 24 heures consécutives en plus du repos quotidien."
     ],
     "Pages 9-11"),

    ("Fiche 11 : Droit à la Formation",
     [
         "- Compte Personnel de Formation (CPF) : Crédit d'heures accumulées.",
         "- Plan de développement des compétences : Obligations de l'employeur.",
         "- Congé de formation : Droit à un congé pour suivre une formation longue durée.",
         "- Validation des acquis de l'expérience (VAE) : Reconnaissance des compétences."
     ],
     "Pages 10-12"),

    ("Fiche 12 : Rupture Conventionnelle",
     [
         "- Définition : Rupture d'un CDI d'un commun accord.",
         "- Procédure : Entretien, Convention de rupture, Homologation.",
         "- Indemnité spécifique : Minimum équivalent à l’indemnité légale de licenciement.",
         "- Droit au chômage : Allocation chômage après la rupture."
     ],
     "Pages 7-9"),

    ("Fiche 13 : Sécurité Sociale et Protection Sociale",
     [
         "- Sécurité sociale : Système de protection couvrant les risques sociaux.",
         "- Branches principales : Maladie, Famille, Vieillesse, Accidents du travail.",
         "- Protection complémentaire : Mutuelles, prévoyance."
     ],
     "Pages 13-15"),

    ("Fiche 14 : Santé et Sécurité au Travail",
     [
         "- Obligations de l'employeur : Évaluation des risques, Formation à la sécurité.",
         "- Droit de retrait : Danger grave et imminent.",
         "- Accidents du travail : Déclaration obligatoire."
     ],
     "Pages 8-12")
]

# Write the content to the canvas
for title, content, page_range in fiches:
    can.drawString(20, 750, title)
    y = 730
    for line in content:
        can.drawString(20, y, line)
        y -= 15
    can.drawString(20, y - 10, f"Références approximatives dans le document : {page_range}")
    can.showPage()

# Finalize the PDF
can.save()

# Move to the beginning of the StringIO buffer
packet.seek(0)

# Read the existing PDF
new_pdf = PdfReader(packet)
output = PdfWriter()

# Add the new page to the existing document
for page in new_pdf.pages:
    output.add_page(page)

# Write the final PDF to a file
with open(output_pdf_path, "wb") as outputStream:
    output.write(outputStream)

output_pdf_path
