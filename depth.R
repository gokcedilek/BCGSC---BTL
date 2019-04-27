library("optparse")
library("ggplot2")
library("dplyr")
#this script plots the mx and bx depth with input files and input parameters for the values of n, N, c, C, and saves the plots to separate files

options <- list(make_option(c("-b", "--barcodeDepth"), help = "barcode depth file"),
                make_option(c("-m", "--minimizerDepth"), help = "minimizer depth file"),
                make_option(c("-n", "--minN"), type = "integer", help = " "),
                make_option(c("-N", "--maxN"), type = "integer", help = " "),
                make_option(c("-c", "--minC"), type = "integer", help = " "),
                make_option(c("-C", "--maxC"), type = "integer", help = " ") )

arguments <- parse_args(OptionParser(usage = "%prog [options] file file", 
                                     option_list=options,
                                     prog = "plot bx-mx depth"))

# for (opt in arguments) {
#   if (opt.isnull) {
#     printf("%s is null\n", opt)
#exit?
#   }
# }
# 

path_bd<-sprintf("%s/%s", getwd(), arguments$barcodeDepth)
key<-strsplit(arguments$barcodeDepth, "[.]")[[1]][1]
pdf(paste0(key,".bdepth.pdf"))
bd<-read.csv(path_bd, header=T)

labels<-tibble(x=c(arguments$minN,arguments$maxN), text=c(paste("n = ", arguments$minN), paste("N = ", arguments$maxN)))
ggplot(bd) +
  aes(x = Barcode.Depth) +
  geom_histogram(binwidth = 1, boundary = 0)+ xlab("# of Minimizers") +ylab("# of Barcodes")+
  theme_bw() + ggtitle(paste(key, " n = ", arguments$minN, " N = ", arguments$maxN, " : # of Minimizers per Barcode "))+
  geom_vline(xintercept=c(arguments$minN,arguments$maxN), linetype = "dashed") +
  geom_text(data=labels, aes(x=x, y = 1000, label=text), size=5, hjust=0.5, vjust=0.5) +
  theme(text=element_text(size=14))
dev.off()
#need to ask about y = 1000!!!

path_md<-sprintf("%s/%s", getwd(), arguments$minimizerDepth)
key<-strsplit(arguments$minimizerDepth, "[.]")[[1]][1]
pdf(paste0(key,".mdepth.pdf"))
md<-read.csv(path_md, header=T)

labels<-tibble(x=c(arguments$minC,arguments$maxC), text=c(paste("c = ", arguments$minC), paste("C = ", arguments$maxC)))
ggplot(md) +
  aes(x = Minimizer.Depth) +
  geom_histogram(binwidth = 1, boundary = 0)+ xlab("# of Barcodes") +ylab("# of Minimizers")+
  theme_bw() + ggtitle(paste(key, " c = ", arguments$minC, " C = ", arguments$maxC, " : # of Barcodes per Minimizer ")) +
  geom_vline(xintercept=c(arguments$minC,arguments$maxC), linetype = "dashed") +
  geom_text(data=labels, aes(x=x, y=1000, label=text), size=5, hjust=0.5, vjust=0.5)+
  theme(text=element_text(size=14)) +ylim(0,1000)
dev.off()
#need to ask about y = 1000!!!



#this does the job -- but I need to figure out how to arrange x and y limits!!! because the data is only visible in a certain range ...


