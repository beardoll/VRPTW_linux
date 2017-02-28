% ��ȡxml�ļ�
fileName = 'RC103_100.xml';
filePath = 'C:\Users\cfinsbear\Documents\DVRPBTW\DVRPBTW\ALNS_Result\';
try
    xDoc = xmlread([filePath, fileName]);
catch
    error('Failed to read XML file %s', infilename);
end

routeLen = xDoc.getElementsByTagName('RouteLen');  
% ·������
% ���ص���һ��elementList, ��ʹֻ��һ���ڵ㣬Ҳ��ҪrouteLen.item(0)����ȡ�ýڵ�

routeSet = [];

allRoute = xDoc.getElementsByTagName('Route');
for i = 0:allRoute.getLength-1
    thisRoute = allRoute.item(i);
    routeNode.id = str2num(char(thisRoute.getAttribute('index')));
    routeNode.nodeSet = [];
    Node = thisRoute.getFirstChild;
    while ~isempty(Node)
        if Node.getNodeType == Node.ELEMENT_NODE
            % Assume that each element has a single org.w3c.dom.Text child
            % �е�childnode����dom���͵�
            % xmlread����Ϊ�ո�Ҳ����һ���ڵ㣬�����Ҫ�����ж�
            nodeElem.id = str2num(char(Node.getAttribute('id')));
            nodeElem.type = char(Node.getAttribute('type'));
            coordinate = Node.getFirstChild;
            while ~isempty(coordinate)
                % ��ȡx, y���꣬��childNode�ķ�ʽ��ȡ
                if coordinate.getNodeType == coordinate.ELEMENT_NODE
                    s = char(coordinate.getFirstChild.getData);
                    switch char(coordinate.getTagName)
                        case 'cx'
                            nodeElem.cx = str2num(s);
                        case 'cy'
                            nodeElem.cy = str2num(s);
                    end
                end
                coordinate = coordinate.getNextSibling;
            end
            routeNode.nodeSet = [routeNode.nodeSet, nodeElem];
        end
        Node = Node.getNextSibling;
    end
    routeSet = [routeSet, routeNode];
end

len = str2num(char(routeLen.item(0).getFirstChild.getData));

drawRoute(routeSet);
