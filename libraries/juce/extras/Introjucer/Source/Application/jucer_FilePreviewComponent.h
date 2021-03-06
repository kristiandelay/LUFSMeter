/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-11 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

#ifndef __JUCER_FILEPREVIEWCOMPONENT_JUCEHEADER__
#define __JUCER_FILEPREVIEWCOMPONENT_JUCEHEADER__


//==============================================================================
/**
*/
class ItemPreviewComponent  : public Component
{
public:
    ItemPreviewComponent (const File& f)
        : file (f)
    {
        setOpaque (true);
        tryToLoadImage();
    }

    void paint (Graphics& g)
    {
        IntrojucerLookAndFeel::fillWithBackgroundTexture (*this, g);

        if (drawable != nullptr)
        {
            Rectangle<int> area = RectanglePlacement (RectanglePlacement::centred | RectanglePlacement::onlyReduceInSize)
                                    .appliedTo (drawable->getBounds(), Rectangle<int> (4, 22, getWidth() - 8, getHeight() - 26));

            Path p;
            p.addRectangle (area);
            DropShadow (Colours::black.withAlpha (0.5f), 6, Point<int> (0, 1)).drawForPath (g, p);

            g.fillCheckerBoard (area, 24, 24, Colour (0xffffffff), Colour (0xffeeeeee));

            g.setOpacity (1.0f);
            drawable->drawWithin (g, area.toFloat(), RectanglePlacement::stretchToFit, 1.0f);
        }

        g.setFont (Font (14.0f, Font::bold));
        g.setColour (findColour (mainBackgroundColourId).contrasting());
        g.drawMultiLineText (facts.joinIntoString ("\n"), 10, 15, getWidth() - 16);
    }

private:
    StringArray facts;
    File file;
    ScopedPointer<Drawable> drawable;

    void tryToLoadImage()
    {
        facts.clear();
        facts.add (file.getFullPathName());
        drawable = nullptr;

        {
            ScopedPointer <InputStream> input (file.createInputStream());

            if (input != nullptr)
            {
                const int64 totalSize = input->getTotalLength();

                String formatName;
                if (ImageFileFormat* format = ImageFileFormat::findImageFormatForStream (*input))
                    formatName = " " + format->getFormatName();

                input = nullptr;

                Image image (ImageCache::getFromFile (file));

                if (image.isValid())
                {
                    DrawableImage* d = new DrawableImage();
                    d->setImage (image);
                    drawable = d;

                    facts.add (String (image.getWidth()) + " x " + String (image.getHeight()) + formatName);
                }

                if (totalSize > 0)
                    facts.add (File::descriptionOfSizeInBytes (totalSize));
            }
        }

        if (drawable == nullptr)
        {
            ScopedPointer<XmlElement> svg (XmlDocument::parse (file));

            if (svg != nullptr)
                drawable = Drawable::createFromSVG (*svg);
        }

        facts.removeEmptyStrings (true);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ItemPreviewComponent)
};


#endif   // __JUCER_FILEPREVIEWCOMPONENT_JUCEHEADER__
