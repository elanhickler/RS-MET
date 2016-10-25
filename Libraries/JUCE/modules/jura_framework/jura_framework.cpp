#ifdef JURA_FRAMEWORK_H_INCLUDED
 /* When you add this cpp file to your project, you mustn't include it in a file where you've
    already included any other headers - just put it inside a file on its own, possibly with your config
    flags preceding it, but don't include anything else. That also includes avoiding any automatic prefix
    header files that the compiler may be using.
 */
 #error "Incorrect use of JUCE cpp file"
#endif

#include "jura_framework.h"

namespace jura
{

#include "control/jura_Parameter.cpp"
#include "control/jura_AutomatableParameter.cpp"
#include "control/jura_AutomatableModule.cpp"
#include "control/jura_StateManager.cpp"
// maybe this control stuff can be done in RAPT to make it available to non-JUCE based client code, 
// too

#include "tools/jura_MiscTools.cpp"
#include "tools/jura_StringTools.cpp"
#include "tools/jura_XmlTools.cpp"

#include "files/jura_AudioFileInfo.cpp"
#include "files/jura_FileManager.cpp"
#include "files/jura_StateFileManager.cpp"
#include "files/jura_AudioFileManager.cpp"
#include "files/jura_ImageFileManager.cpp"

#include "gui/graphics/jura_ColourAHSL.cpp"
#include "gui/graphics/jura_ColourizableBitmap.cpp"
#include "gui/graphics/jura_ColourScheme.cpp"
#include "gui/graphics/jura_BitmapFonts.cpp"
#include "gui/graphics/jura_GraphicsTools.cpp"

#include "gui/misc/jura_DescribedComponent.cpp"
#include "gui/misc/jura_ColourSchemeComponent.cpp"
#include "gui/misc/jura_RectangleComponent.cpp"

#include "gui/widgets/jura_RWidget.cpp"
#include "gui/widgets/jura_WidgetSet.cpp"
#include "gui/widgets/jura_RTextField.cpp"
#include "gui/widgets/jura_RTextEditor.cpp"
#include "gui/widgets/jura_RButton.cpp"
#include "gui/widgets/jura_RScrollBar.cpp"
#include "gui/widgets/jura_RTreeView.cpp"
#include "gui/widgets/jura_RPopUpComponent.cpp"
#include "gui/widgets/jura_RPopUpMenu.cpp"
#include "gui/widgets/jura_RSlider.cpp"
#include "gui/widgets/jura_RComboBox.cpp"
#include "gui/widgets/jura_StateLoadSaveWidgetSet.cpp"

#include "gui/plots/jura_CoordinateSystemOld.cpp"
#include "gui/plots/jura_CurveFamilyPlotOld.cpp" 
#include "gui/plots/jura_SpectrumDisplayOld.cpp"

#include "gui/dialogs/jura_RDialogBox.cpp"
#include "gui/dialogs/jura_ColourSchemeSetupDialog.cpp"

#include "gui/editors/jura_Editor.cpp"

}
