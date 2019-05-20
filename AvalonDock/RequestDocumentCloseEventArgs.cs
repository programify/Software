using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.ComponentModel;

namespace AvalonDock
{
    /// <summary>
    /// 
    /// </summary>
    public class RequestDocumentCloseEventArgs : CancelEventArgs
    {
        /// <summary>
        /// 
        /// </summary>
        public RequestDocumentCloseEventArgs(DocumentContent doc)
        {
            DocumentToClose = doc;
        }

        /// <summary>
        /// Document content that user wants to close
        /// </summary>
        public DocumentContent DocumentToClose { get; private set; }
    }
}
